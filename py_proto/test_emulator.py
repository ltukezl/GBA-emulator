import pytest

import emulator

def test_rotater():
    cprs = emulator.CPRS()
    ror = emulator.Ror(cprs)

    actual_value = ror.shift(sourceValue=1, shiftAmount=2)

    assert actual_value == 1


def test_rotater_cprs_manipulations():
    cprs = emulator.CPRS()
    ror = emulator.Ror(cprs)

    ror.calcConditions(result=4, sourceValue=1, shiftAmount=2)

    assert cprs == emulator.CPRS(carry=1, negative=0, zero=1)


if __name__ == "__main__":
    pytest.main([__file__])