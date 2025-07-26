project('Gba-emulator', 'cpp', default_options : ['cpp_std=c++latest', 'warning_level=1'])

add_project_arguments(['-fbracket-depth=3072'], language : 'cpp')
compiler = meson.get_compiler('cpp')

root_path = meson.current_source_dir().replace('\\', '\\\\') 
add_project_arguments('-DROOT_PATH="' + root_path + '"', language: 'cpp')

sfml_include = 'D:/SFML-3.0.1/include'

if get_option('buildtype') == 'debug'
  sfml_dependencies = declare_dependency(
  include_directories: include_directories(sfml_include),
  link_args: [
    '-LD:/SFML-3.0.1/build/lib/Debug',  # Link directory
    '-lsfml-main-d',
    '-lsfml-graphics-d',
    '-lsfml-window-d',
    '-lsfml-system-d',
  ]
)

elif get_option('buildtype') == 'release'
  sfml_dependencies = declare_dependency(
  include_directories: include_directories(sfml_include),
  link_args: [
    '-LD:/SFML-3.0.1/build/lib/Release',  # Link directory
    '-lsfml-main',
    '-lsfml-graphics',
    '-lsfml-window',
    '-lsfml-system',
    '-lsfml-network',
  ]
)
endif

sources = [
    'src/Gbacpu.cpp',
    'src/Timer/timers.cpp',
    'src/Thumb/ThumbOpCodes.cpp',
    'src/Memory/memoryMappedIO.cpp',
    'src/Memory/MemoryOps.cpp',
    'src/Interrupt/Interrupt.cpp',
    'src/DMA/DMA.cpp',
    'src/Display/disassembler.cpp',
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

incdir = include_directories('include')

executable('Gba-emulator', sources, include_directories : [incdir], dependencies: sfml_dependencies)
