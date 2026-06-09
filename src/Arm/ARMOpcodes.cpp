#include <array>
#include <cstdint>
#include <string>
#include <utility>

#include "Arm/armopcodes.h"
#include "Arm/ArmOpcodes/BlockDataTransferLoads.hpp"
#include "Arm/ArmOpcodes/BlockDataTransferStores.hpp"
#include "Arm/ArmOpcodes/Branch.hpp"
#include "Arm/ArmOpcodes/DataProcessingImmediate.hpp"
#include "Arm/ArmOpcodes/Multiply.hpp"
#include "Arm/ArmOpcodes/SDDHelper.hpp"
#include "Arm/ArmOpcodes/Undefop.hpp"
#include "CommonOperations/arithmeticOps.h"
#include "CommonOperations/conditions.h"
#include "CommonOperations/logicalOps.h"
#include "cplusplusRewrite/BarrelShifterDecoder.h"
#include "cplusplusRewrite/HwRegisters.h"
#include "Display/Disassembler.hpp"
#include "GBAcpu.h"
#include "Interrupt/interrupt.h"
#include "Memory/memoryOps.h"

void lslCond(int& saveTo, int from, int immidiate)
{
    uint64_t tmp = (unsigned)from;
    saveTo = tmp << immidiate;

    if (immidiate > 32) {
        r.m_cpsr.carry = 0;
    } else if (immidiate > 0) {
        r.m_cpsr.carry = ((unsigned)tmp >> (32 - immidiate) & 1);
    }
    negative(saveTo);
    zero(saveTo);
}

void lsrCond(int& saveTo, int from, int immidiate)
{
    uint64_t tmp = (unsigned)from;
    saveTo = tmp >> immidiate;

    if (immidiate > 0) {
        r.m_cpsr.carry = (tmp >> (immidiate - 1) & 1);
    }
    negative(saveTo);
    zero(saveTo);
}

void asrCond(int& saveTo, int from, int immidiate)
{
    int64_t tmp = from;
    saveTo = tmp >> immidiate;

    if (immidiate != 0) {
        r.m_cpsr.carry = (tmp >> (immidiate - 1) & 1);
    }
    zero(saveTo);
    negative(saveTo);
}

void rorCond(int& saveTo, int from, int immidiate)
{
    if (immidiate > 32) {
        rorCond(saveTo, from, immidiate - 32);
    } else {
        if (immidiate > 0) {
            r.m_cpsr.carry = (from >> (immidiate - 1) & 1);
        }
        saveTo = ((unsigned)from >> immidiate) |
            ((unsigned)from << (32 - immidiate));
        negative(saveTo);
        zero(saveTo);
    }
}

void lslNoCond(int& saveTo, int from, int immidiate)
{
    uint64_t tmp = (unsigned)from;
    saveTo = tmp << immidiate;
}

void lsrNoCond(int& saveTo, int from, int immidiate)
{
    uint64_t tmp = (unsigned)from;
    saveTo = tmp >> immidiate;
}

void asrNoCond(int& saveTo, int from, int immidiate)
{
    int64_t tmp = from;
    saveTo = tmp >> immidiate;
}

void rorNoCond(int& saveTo, int from, int immidiate)
{
    if (immidiate > 32) {
        rorNoCond(saveTo, from, immidiate - 32);
    } else {
        saveTo = ((unsigned)from >> immidiate) |
            ((unsigned)from << (32 - immidiate));
    }
}

uint32_t RORnoCond(uint32_t immediate, uint32_t by)
{
    if (by > 32) {
        RORnoCond(immediate, by - 32);
    }
    return (immediate >> by) | (immediate << (32 - by));
}

static void rrx(int& saveTo, uint32_t from, bool conditions)
{
    saveTo = (r.m_cpsr.carry << 31) | (from >> 1);
    if (conditions) {
        r.m_cpsr.carry = from & 1;
        zero(saveTo);
        negative(saveTo);
    }
}

void (*ARMshifts[4])(int&, int, int) = {lslCond, lsrCond, asrCond, rorCond};
void (*ARMshiftsNoCond[4])(int&, int, int) = {lslNoCond, lsrNoCond, asrNoCond,
                                              rorNoCond};
std::string ARMshifts_s[4] = {"lsl", "lsr", "asr", "ror"};

void updateMode()
{
    // std::cout << "switched mode to " << mode << std::endl;
    switch (r.m_cpsr.mode) {
        case CpuModes_t::EUSR: r.updateMode(CpuModes_t::EUSR); break;
        case CpuModes_t::EFIQ: r.updateMode(CpuModes_t::EFIQ); break;
        case CpuModes_t::EIRQ: r.updateMode(CpuModes_t::EIRQ); break;
        case CpuModes_t::ESUPER: r.updateMode(CpuModes_t::ESUPER); break;
        case CpuModes_t::EABORT: r.updateMode(CpuModes_t::EABORT); break;
        case CpuModes_t::EUNDEF: r.updateMode(CpuModes_t::EUNDEF); break;
        case CpuModes_t::ESYS: r.updateMode(CpuModes_t::ESYS); break;
    }
}

void msr(int& saveTo, int operand1, int operand2)
{
    r.m_cpsr.val = operand2;
    updateMode();
}

void msr2(int& saveTo, int operand1, int operand2)
{
    r[16] = operand2;
    updateMode();
}

void MSR(uint32_t opCode)
{
    bool SPSR = (opCode >> 22) & 1;
    CPSR_t tmp_cpsr{};
    uint8_t rotate = (opCode >> 8) & 0xF;
    uint32_t imm = opCode & 0xFF;
    uint32_t shiftedImm = RORnoCond(imm, rotate);
    shiftedImm = RORnoCond(shiftedImm, rotate);
    tmp_cpsr.val = shiftedImm;

    if (SPSR) {
        r[16] = shiftedImm;
    } else {
        r.m_cpsr.zero = tmp_cpsr.zero;
        r.m_cpsr.overflow = tmp_cpsr.overflow;
        r.m_cpsr.carry = tmp_cpsr.carry;
        r.m_cpsr.negative = tmp_cpsr.negative;
    }
}

void mrs(int& saveTo, int operand1, int operand2)
{ saveTo = r.m_cpsr.val; }

void mrs2(int& saveTo, int operand1, int operand2)
{ saveTo = r[16]; }

void (*dataOperations[0x20])(int&, int, int) = {
    And,  Ands, Eor,  Eors, Sub,  Subs, Rsb, Rsbs, Add, Adds, Adc,
    Adcs, Sbc,  Sbcs, Rsc,  Rscs, mrs,  Tst, msr,  Teq, mrs2, Cmp,
    msr2, Cmn,  Orr,  Orrs, Mov,  Movs, Bic, Bics, Mvn, Mvns};

std::string dataOperations_s[0x20] = {
    "and", "ands", "or",  "ors",  "sub", "subs", "rsb", "rsbs",
    "add", "adds", "adc", "adcs", "sbc", "sbcs", "rsc", "rscs",
    "tst", "tst",  "msr", "teq",  "cmp", "cmp",  "msr", "cmn",
    "or",  "ors",  "mov", "movs", "bic", "bics", "mvn", "mvns"};

void immediateRotate(int opCode)
{
    bool codeExecuted = false;
    if (((opCode >> 12) & 0x3FF) == 0x28f && ((opCode >> 23) & 3) == 2 &&
        ((opCode >> 26) & 3) == 0 && !codeExecuted) {
        int sprs = (opCode >> 22) & 1;
        int rm = opCode & 0xF;

        if (sprs) {
            int tmp = r.m_cpsr.val & 0xFFFFFFF;
            tmp |= r[rm] & 0xF0000000;
            r[16] = tmp;
        } else {
            int tmp = r.m_cpsr.val & 0xFFFFFFF;
            tmp |= r[rm] & 0xF0000000;
            r.m_cpsr.val = tmp;
        }
        codeExecuted = true;
    }

    if (!codeExecuted) {
        int rd = (opCode >> 12) & 15; // destination
        int rs = (opCode >> 16) & 15; // first operand
        int rn = opCode & 15;         // 2nd operand
        int tmpRegister = r[rn];
        int immediate = (opCode >> 7) & 0x1F;
        int shiftId = (opCode >> 5) & 3;
        int operationID = (opCode >> 20) & 0x1F;
        int conditions = (opCode >> 20) & 1;
        int operand1 = r[rs];

        if (rn == 15) {
            tmpRegister += 4;
        } else if (rs == 15) {
            operand1 += 4;
        }

        if (shiftId == 3 && immediate == 0) {
            rrx(tmpRegister, tmpRegister, conditions);
        } else {
            if (immediate == 0 && shiftId != 0) {
                immediate = 0x20;
            }
            if (conditions &&
                (((operationID > 3) && (operationID < 16)) ||
                 ((operationID > 21) && (operationID < 24)))) {
                ARMshiftsNoCond[shiftId](tmpRegister, tmpRegister, immediate);
            } else if (conditions) {
                ARMshifts[shiftId](tmpRegister, tmpRegister, immediate);
            } else {
                ARMshiftsNoCond[shiftId](tmpRegister, tmpRegister, immediate);
            }
        }

        dataOperations[operationID]((int&)r[rd], operand1, tmpRegister);

        if (rd == 15 && (opCode >> 20) & 1) {
            r.m_cpsr.val = r[16];
            updateMode();
        }
    }
}

void registerRotate(int opCode)
{
    int rd = (opCode >> 12) & 0xF; // destination
    int rn = (opCode >> 16) & 0xF; // first operand
    int rm = opCode & 0xF;
    int rs = (opCode >> 8) & 0xF;
    int shiftId = (opCode >> 5) & 3;
    int shiftAmount = r[rs] & 0xFF;
    int tmpResult = 0;
    int operationID = (opCode >> 20) & 0x1F;
    int conditions = (opCode >> 20) & 1;

    if (rm == 15) {
        r[rm] += 8;
    } else if (rn == 15) {
        r[rn] += 8;
    }

    if (conditions &&
        (((operationID > 3) && (operationID < 16)) ||
         ((operationID > 21) && (operationID < 24)))) {
        ARMshiftsNoCond[shiftId](tmpResult, r[rm], shiftAmount);
    } else if (conditions) {
        ARMshifts[shiftId](tmpResult, r[rm], shiftAmount);
    } else {
        ARMshiftsNoCond[shiftId](tmpResult, r[rm], shiftAmount);
    }

    dataOperations[operationID]((int&)r[rd], r[rn], tmpResult);

    if (rd == TRegisters::EProgramCounter && (opCode >> 20) & 1) {
        r.m_cpsr.val = r[16];
        updateMode();
    }

    if (rm == 15) {
        r[rm] -= 8;
    } else if (rn == 15) {
        r[rn] -= 8;
    }
}

void dataProcessingImmediate(int opCode)
{
    int rd = (opCode >> 12) & 0xF; // destination
    int rs = (opCode >> 16) & 0xF; // first operand
    int operand1 = r[rs];
    if (rs == TRegisters::EProgramCounter) {
        operand1 += 4;
    }
    bool conditions = (opCode >> 20) & 1;
    int operationID = (opCode >> 20) & 0x1F;
    int operation = (opCode >> 21) & 0xF;

    const bool isLogicalOp = (operation == 0b0000) || (operation == 0b0001) ||
        (operation == 0b1000) || (operation == 0b1001) ||
        (operation == 0b1100) || (operation == 0b1101) ||
        (operation == 0b1110) || (operation == 0b1111);
    const auto func = BarrelShifterDecoder::decode(opCode);
    const uint32_t result = func(r, opCode, conditions && isLogicalOp);
    dataOperations[operationID]((int&)r[rd], operand1, result);

    if (rd == 15 && conditions) {
        r.m_cpsr.val = r[16];
        updateMode();
    }
}

static void null_func(Registers&, const uint32_t) {}

static uint32_t constexpr reduce_opcode(const uint32_t opCode)
{
    const uint32_t low_byte = (opCode >> 4) & 0xF;
    const uint32_t high_byte = (opCode >> 16) & 0xFF0;
    return low_byte | high_byte;
}

template<uint32_t opCode>
static auto constexpr decode_arm_opcode()
{
    if constexpr (UndefOp::isThisOpcode(opCode)) {
        return &UndefOp::execute;
    }
    if constexpr (DataProcessingImmediate::isThisOpcode(opCode)) {
        return &DataProcessingImmediate::execute<DataProcessingImmediate::mask(
            opCode)>;
    }
    if constexpr (((opCode >> 26) & 0x3) == 0) {
        return HalfDataTransfer::decode_hdd<opCode>();
    }
    if constexpr (((opCode >> 26) & 0x3) == 1) {
        return SingleDataTransfer::decode_sdd<opCode>();
    }
    if constexpr (BlockDataTransfer::isThisOpcode(opCode)) {
        if constexpr (BlockDataTransferLoad::isThisOpcode(opCode)) {
            return BlockDataTransferLoad::execute<BlockDataTransfer::mask(
                opCode)>;
        }
        if constexpr (BlockDataTransferStore::isThisOpcode(opCode)) {
            return BlockDataTransferStore::execute<BlockDataTransfer::mask(
                opCode)>;
        }
    }
    if constexpr (branches::ArmBranch::isThisOpcode(opCode)) {
        return branches::ArmBranch::execute<branches::ArmBranch::mask(opCode)>;
    }

    return &null_func;
}

static consteval auto index_to_opcode(const uint32_t opcode)
{
    const uint32_t low_bits = opcode & 0xF;
    const uint32_t high_bits = opcode & 0xFF0;
    return (high_bits << 16) | (low_bits << 4);
}

template<typename T, std::size_t... Is>
consteval void insert_opcodes(T& arr, std::index_sequence<Is...>)
{ ((arr[Is] = decode_arm_opcode<index_to_opcode(Is)>()), ...); }

static constexpr std::array m_dispatch_table = {[]() consteval {
    std::array<void (*)(Registers&, const uint32_t), 0xC00> tmp{};
    insert_opcodes(tmp, std::make_index_sequence<tmp.size()>{});
    return tmp;
}()};

#include "Arm/json.hpp"
#include <fstream>
#include <iostream>
#include <print>

uint32_t tst_num = 0;

void write_initial_memory_transactions(const auto& json)
{
    for (const auto& j: json["transactions"]) {
        if (j["addr"] == j["data"]) {
            continue;
        }
        if (j["kind"] == 0 || j["kind"] == 1) {
            if (j["size"] == 4) {
                writeToAddress32(j["addr"].template get<uint32_t>() & ~3,
                                 j["data"]);
            } else if (j["size"] == 2) {
                writeToAddress16(j["addr"].template get<uint32_t>() & ~1,
                                 j["data"]);
            } else if (j["size"] == 1) {
                writeToAddress(j["addr"].template get<uint32_t>(), j["data"]);
            }
        }
    }
}

void write_cpu_registers(auto& dst, const auto& values)
{
    size_t idx = 0;
    for (const auto& initial_R: values) {
        dst[idx] = initial_R.template get<uint32_t>();
        idx++;
    }
}

void setup_cpu_registers(auto& registers, const auto& json)
{
    const auto& initials = json["initial"];
    size_t idx = 0;
    for (auto& initial_R: initials["R"]) {
        *registers.usrSys[idx] = initial_R.template get<uint32_t>();
        idx++;
    }

    registers[15] = json["base_addr"][0].template get<uint32_t>();

    write_cpu_registers(registers.fiqBanked, initials["R_fiq"]);
    write_cpu_registers(registers.svcBanked, initials["R_svc"]);
    write_cpu_registers(registers.abtBanked, initials["R_abt"]);
    write_cpu_registers(registers.irqBanked, initials["R_irq"]);
    write_cpu_registers(registers.undBanked, initials["R_und"]);

    registers.m_cpsr.val = initials["CPSR"].template get<uint32_t>();
    registers.updateMode(registers.getMode());

    registers.sprs_usr = 0;
    registers.sprs_fiq = initials["SPSR"][0].template get<uint32_t>();
    registers.sprs_svc = initials["SPSR"][1].template get<uint32_t>();
    registers.sprs_abt = initials["SPSR"][2].template get<uint32_t>();
    registers.sprs_irq = initials["SPSR"][3].template get<uint32_t>();
    registers.sprs_udf = initials["SPSR"][4].template get<uint32_t>();
}

void execute_instruction(auto& registers, const auto& json, const bool failed)
{
    const auto& initials = json["initial"];
    uint32_t opcode = initials["pipeline"][0].template get<uint32_t>();

    int condition = (opcode >> 28) & 0xF;

    registers[15] += 4;
    if (conditions[condition](registers)) {
        if (failed) {
            std::println("\n\nopcode {} ({:x}), reduced {} ({:x})", opcode,
                         opcode, reduce_opcode(opcode), reduce_opcode(opcode));
        }
        std::cout << tst_num << " " << registers[15] << " " << registers[0]
                  << " " << Disassembler::arm_disassembly(registers[15], opcode)
                  << "\n";
        const auto func = m_dispatch_table[reduce_opcode(opcode)];
        func(registers, opcode);
        tst_num++;
    }
    registers[15] += 8;
}

bool validate_final_register_bank(const auto& registers,
                                  const auto& final_values,
                                  const std::string_view register_name)
{
    size_t idx = 0;
    for (const auto& initial_R: final_values) {
        const auto i = registers[idx];
        const auto ex = initial_R.template get<uint32_t>();
        if (i != ex) {
            std::println("from {} register {} got {} expected {}",
                         register_name, idx, i, ex);
            return true;
        }
        idx++;
    }
    return false;
}

void print_cprs(const auto got, const auto expected, const auto str)
{
    std::println("{} register failed got: {} (0x{:x}) expected {} (0x{:x})",
                 str, got, got, expected, expected);
    CPSR_t val_one;
    CPSR_t val_two;
    val_one.val = got;
    val_two.val = expected;
    std::println("zero {} expected {}", static_cast<uint32_t>(val_one.zero),
                 static_cast<uint32_t>(val_two.zero));
    std::println("carry {} expected {}", static_cast<uint32_t>(val_one.carry),
                 static_cast<uint32_t>(val_two.carry));
    std::println("negative {} expected {}",
                 static_cast<uint32_t>(val_one.negative),
                 static_cast<uint32_t>(val_two.negative));
    std::println("overflow {} expected {}",
                 static_cast<uint32_t>(val_one.overflow),
                 static_cast<uint32_t>(val_two.overflow));
    std::println("thumb {} expected {}", static_cast<uint32_t>(val_one.thumb),
                 static_cast<uint32_t>(val_two.thumb));
    std::println("FIQDisable {} expected {}",
                 static_cast<uint32_t>(val_one.FIQDisable),
                 static_cast<uint32_t>(val_two.FIQDisable));
    std::println("IRQDisable {} expected {}",
                 static_cast<uint32_t>(val_one.IRQDisable),
                 static_cast<uint32_t>(val_two.IRQDisable));
    std::println("mode {} expected {}", static_cast<uint32_t>(val_one.mode),
                 static_cast<uint32_t>(val_two.mode));
}

bool validate_result(const auto& registers, const auto& json)
{
    const auto& finals = json["final"];

    bool failed = false;
    size_t idx = 0;
    for (const auto& initial_R: finals["R"]) {
        const auto i = *registers.usrSys[idx];
        const auto ex = initial_R.template get<uint32_t>();
        if (i != ex) {
            std::println(
                "from usrSys register {} got {} (0x{:x}) expected {} (0x{:x})",
                idx, i, i, ex, ex);
            failed = true;
        }
        idx++;
    }

    failed |= validate_final_register_bank(registers.fiqBanked, finals["R_fiq"],
                                           "R_fiq");
    failed |= validate_final_register_bank(registers.svcBanked, finals["R_svc"],
                                           "R_svc");
    failed |= validate_final_register_bank(registers.abtBanked, finals["R_abt"],
                                           "R_abt");
    failed |= validate_final_register_bank(registers.irqBanked, finals["R_irq"],
                                           "R_irq");
    failed |= validate_final_register_bank(registers.undBanked, finals["R_und"],
                                           "R_und");

    if ((registers.m_cpsr.val & 0xf000'00ff) !=
        (finals["CPSR"].template get<uint32_t>() & 0xf000'00ff)) {
        print_cprs(registers.m_cpsr.val & 0xf000'00ff,
                   finals["CPSR"].template get<uint32_t>() & 0xf000'00ff,
                   "m_cpsr");
        failed = true;
    }
    if ((registers.sprs_fiq & 0xf000'00ff) !=
        (finals["SPSR"][0].template get<uint32_t>() & 0xF000'00FF)) {
        print_cprs(registers.sprs_fiq & 0xf000'00ff,
                   finals["SPSR"][0].template get<uint32_t>() & 0xF000'00FF,
                   "sprs_fiq");
        failed = true;
    }
    if ((registers.sprs_svc & 0xf000'00ff) !=
        (finals["SPSR"][1].template get<uint32_t>() & 0xF000'00FF)) {
        print_cprs(registers.sprs_svc & 0xf000'00ff,
                   finals["SPSR"][1].template get<uint32_t>() & 0xF000'00FF,
                   "sprs_svc");
        failed = true;
    }
    if ((registers.sprs_abt & 0xf000'00ff) !=
        (finals["SPSR"][2].template get<uint32_t>() & 0xF000'00FF)) {
        print_cprs(registers.sprs_abt & 0xf000'00ff,
                   finals["SPSR"][2].template get<uint32_t>() & 0xF000'00FF,
                   "sprs_abt");

        failed = true;
    }
    if ((registers.sprs_irq & 0xf000'00ff) !=
        (finals["SPSR"][3].template get<uint32_t>() & 0xF000'00FF)) {
        print_cprs(registers.sprs_irq & 0xf000'00ff,
                   finals["SPSR"][3].template get<uint32_t>() & 0xF000'00FF,
                   "sprs_irq");
        failed = true;
    }
    if ((registers.sprs_udf & 0xf000'00ff) !=
        (finals["SPSR"][4].template get<uint32_t>() & 0xF000'00FF)) {
        print_cprs(registers.sprs_udf & 0xf000'00ff,
                   finals["SPSR"][4].template get<uint32_t>() & 0xF000'00FF,
                   "sprs_udf");
        failed = true;
    }

    return failed;
}

bool validate_memory_regions_final(const auto& json)
{
    for (const auto& j: json["transactions"]) {
        if (j["kind"] == 2) {
            uint32_t val = 0;
            if (j["size"] == 4) {
                val =
                    loadFromAddress32(j["addr"].template get<uint32_t>() & ~3);
            } else if (j["size"] == 2) {
                val =
                    loadFromAddress16(j["addr"].template get<uint32_t>() & ~1);
            } else if (j["size"] == 1) {
                val = loadFromAddress(j["addr"].template get<uint32_t>());
            }
            const auto expected = j["data"].template get<uint32_t>();

            if (val != expected) {
                std::println("from {} got {} expected {}",
                             j["addr"].template get<uint32_t>(), val, expected);
                return true;
            }
        }
    }
    return false;
}

void runSingleStepTests_a()
{
    Registers registers;
    bool failed = false;
    const std::string game = "../ARM7TDMI/v1/arm_msr_imm.json";
    std::ifstream ifs(game);
    const auto jf = nlohmann::json::parse(ifs);
    for (const auto& json: jf) {

        write_initial_memory_transactions(json);
        setup_cpu_registers(registers, json);
        execute_instruction(registers, json, failed);
        failed |= validate_memory_regions_final(json);
        failed |= validate_result(registers, json);
        if (failed) {
            write_initial_memory_transactions(json);
            setup_cpu_registers(registers, json);
            execute_instruction(registers, json, failed);
            validate_memory_regions_final(json);
            validate_result(registers, json);
            return;
        }
    }
}

void ARMExecute(int opCode)
{
    runSingleStepTests_a();
    return;

    int condition = (opCode >> 28) & 0xF;
    cycles += 1;

    if (conditions[condition](r)) // condition true
    {
        if (((opCode >> 26) & 0x3) == 1) {
            m_dispatch_table[reduce_opcode(opCode)](r, opCode);
            // std::println("{}", SingleDataTransfer::disassemble(opCode));
            return;
        }

        if (branches::ArmBranch::isThisOpcode(opCode)) {
            m_dispatch_table[reduce_opcode(opCode)](r, opCode);
            return;
        }

        if (branches::ArmBranchAndExhange::isThisOpcode(opCode)) {
            branches::ArmBranchAndExhange::execute(r, opCode);
            // std::println("{}",
            // branches::ArmBranchAndExhange::disassemble(opCode));
            return;
        }

        if (MultiplyAccumulate::isThisOpcode(opCode)) {
            MultiplyAccumulate::execute(r, opCode);
            // std::println("{}", MultiplyAccumulate::disassemble(opCode));
            return;
        }

        if (MultiplyLong::isThisOpcode(opCode)) {
            MultiplyLong::execute(r, opCode);
            // std::println("{}", MultiplyLong::disassemble(opCode));
            return;
        }

        int opCodeType = (opCode >> 24) & 0xF;
        switch (opCodeType) {
            case 15: interruptController(); break;
            case 14: // coProcessor data ops / register transfer, not used in
                     // GBA
                break;
            case 13:
            case 12: // co processor data transfer, not used in GBA
                break;
            case 9: // block data transfer pre offset. maybe implement S bits
                m_dispatch_table[reduce_opcode(opCode)](r, opCode);
                break;
            case 8: // block data transfer post offset
                m_dispatch_table[reduce_opcode(opCode)](r, opCode);
                break;
            case 5: // single data transfer, immediate pre offset
                break;
            case 4: // single data transfer, immediate post offset
                break;
            case 3:
            case 2: // data processing, immediate check msr?
                if ((((opCode >> 12) & 0x3FF) == 0x28F) &&
                    (((opCode >> 23) & 0x3) == 2) &&
                    (((opCode >> 26) & 0x3) == 0)) {
                    MSR(opCode); //<-
                    // units[ProcessingUnits::EDataProcessing]->execute(opCode);
                } else {
                    dataProcessingImmediate(opCode);
                    // units[ProcessingUnits::EDataProcessing]->execute(opCode);
                }
                break;
            case 1:
            case 0: // data prceossing, multiply, data transfer, branch and
                    // exhange
                if (((opCode >> 4) & 1) == 0) { // data processing
                    // units[ProcessingUnits::EDataProcessing]->execute(opCode);
                    immediateRotate(opCode);           //<-
                } else if (((opCode >> 7) & 1) == 0) { // data processing
                    // units[ProcessingUnits::EDataProcessing]->execute(opCode);
                    registerRotate(opCode); //<-
                } else if ((((opCode >> 23) & 0x1F) == 2) &&
                           (((opCode >> 4) & 0xFF) == 9)) {
                    m_dispatch_table[reduce_opcode(opCode)](r, opCode);
                } else {
                    m_dispatch_table[reduce_opcode(opCode)](r, opCode);
                }
                break;
        }
    }
}
