project('Gba-emulator', 'cpp', default_options : ['cpp_std=c++23'])

add_project_arguments(['-std=c++23', '-fbracket-depth=3072'], language : 'cpp')
compiler = meson.get_compiler('cpp')


sources = [
    'src/Gbacpu.cpp',
    'src/Timer/timers.cpp',
    'src/Thumb/ThumbOpCodes.cpp',
    'src/Memory/memoryMappedIO.cpp',
    'src/Memory/MemoryOps.cpp',
    'src/Interrupt/Interrupt.cpp',
    'src/DMA/DMA.cpp',
    'src/Display/disassembler.cpp',
    'src/Display/Display.cpp',
    'src/Display/MemoryViewer.cpp',
    'src/Display/Palette/RgbaPalette.cpp',
    'src/Display/Rendermodes/RenderMode3.cpp',
    'src/Display/Rendermodes/RenderMode4.cpp',
    'src/Display/Rendermodes/RenderMode5.cpp',
    'src/Display/Rendermodes/TextMode.cpp',
    'src/Display/Sprites/Sprite.cpp',
    'src/Display/Sprites/SpriteGenerator.cpp',
    'src/Display/Tile/Tile.cpp',
    'src/Display/Tile/Tileset.cpp',
    'src/cplusplusRewrite/barrelShifter.cpp',
    'src/cplusplusRewrite/BarrelShifterDecoder.cpp',
    'src/cplusplusRewrite/dataProcessingOp.cpp',
    'src/cplusplusRewrite/LogicOps.cpp',
    'src/cplusplusRewrite/MathOps.cpp',
    'src/cplusplusRewrite/Operation.cpp',
    'src/cplusplusRewrite/Shifts.cpp',
    'src/CommonOperations/arithmeticOps.cpp',
    'src/CommonOperations/conditions.cpp',
    'src/CommonOperations/GbaStrings.cpp',
    'src/CommonOperations/LogicalOps.cpp',
    'src/Arm/ARMOpcodes.cpp',
]


sfml_include = 'D:\c sources\SFML-2.5.1\include'

sfml_dependencies = declare_dependency(
  include_directories: include_directories(sfml_include),
  link_args: [
    '-LD:\Built libraries\sfml_2_5_1\lib\Debug',  # Link directory
    '-lsfml-graphics-d',
    '-lsfml-window-d',
    '-lsfml-system-d',
  ]
)

incdir = include_directories('include')
sfml_inc = include_directories('D:\c sources\SFML-2.5.1\include')

executable('Gba-emulator', sources, include_directories : [incdir, sfml_inc], dependencies: sfml_dependencies)
