# PCSX2
PCSX2 is a free and open-source PlayStation 2 (PS2) emulator. Its purpose is to emulate the PS2's hardware, using a combination of MIPS CPU [Interpreters](https://en.wikipedia.org/wiki/Interpreter_\(computing\)), [Recompilers](https://en.wikipedia.org/wiki/Dynamic_recompilation) and a [Virtual Machine](https://en.wikipedia.org/wiki/Virtual_machine) which manages hardware states and PS2 system memory. This allows you to play PS2 games on your PC, with many additional features and benefits.

# Fork Details
This is a fork of [#03027453c8](https://github.com/PCSX2/pcsx2/tree/03027453c8fe8cd95164271211e529067bf5b91a) where Sly CRC hacks were still a thing, a PNACH pointer related bugfix was released and performance & graphics were nicely balanced (even for older PCs like mine). This fork is set to enhance the trilogy with cheats and edits to optimize graphics and performance.

## Special thanks:
- [**Meos**](https://www.youtube.com/channel/UCBjGlnrNZmHVLnqePH6A8vQ) - made the legendary **No Motion Blur** cheat for all versions of Sly
- [**Asasega**](https://forums.pcsx2.net/User-asasega) - created a method for patching "sceGsSetHalfOffset" routine which removes screen shakiness/"interlacing"
- [**NiV**](https://github.com/NiV-L-A) - developed a custom code-type to allow conditional cheats for pointers

# Project Details

The PCSX2 project has been running for more than ten years. Past versions could only run a few public domain game demos, but newer versions can run many games at full speed, including popular titles such as s*y cooper

# System Requirements
## Minimum
* OS: Windows 7 or GNU/Linux
* CPU: Any that supports SSE2 @ [1600 STR](#Notes)
* GPU: DirectX 10 support
* RAM: 2GB

## Recommended
* OS: Windows 10 (64-bit) or GNU/Linux (64-bit)
* CPU: Any that supports AVX2 (Core series Haswell or Ryzen and newer) @ [2000 STR](#Notes) or better
* GPU: DirectX 11 support or better
* RAM: 4GB or more

## Best settings:
- Emulation settings -> Speedhacks -> EE Cycle Rate: -3
- Emulation settings -> Speedhacks -> Enable INTC Spin Detection: On
- Emulation settings -> Speedhacks -> Enable Wait Loop Detection: On
- Emulation settings -> Speedhacks -> mVU Flag Hack: On
- Emulation settings -> Speedhacks -> MTVU: On
- Video (GS) -> Renderer: **Direct3D 11 (Hardware)**
- Video (GS) -> Interlacing: None
- Video (GS) -> Large Framebuffer: Off
- Video (GS) -> Mipmapping: Off
- Video (GS) -> CRC Hack Level: Automatic
- Video (GS) -> Blending Accuracy: Basic
## Freely adjustable settings (preference / lag optimization):
#### Visuals:
- Emulation settings -> GS Window -> Aspect Ratio
- Video (GS) -> Texture Filtering
- Video (GS) -> Shader Configuration -> Texture Filtering of Display
#### Performance:
- Emulation settings -> Speedhacks -> EE Cycle Skipping: Higher than 0 (trades slow-mo with choppier gameplay)
- Video (GS) -> Internal Resolution
#### Other:
- Emulation settings -> Speedhacks -> Enable fast CDVD
- Emulation settings -> Game Fixes - Skip MPEG hack

## Notes
- Each game is still missing all shadows on the **Direct3D 11** renderer, but are present when using **OpenGL**. Only downside being other graphical glitches and a big impact on performance.

# Screenshots (4:3)


