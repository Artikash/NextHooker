# Textractor [![Backers on Open Collective](https://opencollective.com/Textractor/backers/badge.svg)](#backers) [![Sponsors on Open Collective](https://opencollective.com/Textractor/sponsors/badge.svg)](#sponsors) 

![How it looks](screenshot.png)

[Español](https://github.com/Artikash/Textractor/blob/master/README_ES.md) ● [简体中文](https://github.com/Artikash/Textractor/blob/master/README_SC.md) ● 
[日本語](https://github.com/Artikash/Textractor/blob/master/README_JP.md) ● [Русский](https://github.com/Artikash/Textractor/blob/master/README_RU.md)

**Textractor** (a.k.a. NextHooker) is an open-source x86/x64 video game text hooker for Windows/Wine based off of [ITHVNR](http://www.hongfire.com/forum/showthread.php/438331-ITHVNR-ITH-with-the-VNR-engine).<br>
Watch the [tutorial video](https://youtu.be/eecEOacF6mw) for a quick rundown on using it.

[![Donate](https://www.paypalobjects.com/en_US/i/btn/btn_donate_SM.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=akashmozumdar%40gmail.com&item_name=Textractor%20development&currency_code=USD)

## Download

Releases of Textractor can be found [here](https://github.com/Artikash/Textractor/releases).<br>
The last release of ITHVNR can be found [here](https://drive.google.com/open?id=13aHF4uIXWn-3YML_k2YCDWhtGgn5-tnO).<br>
Try running vc_redist.x86.exe if you get an error when starting Textractor.

## Features

- Highly extensible
- Auto hook many game engines (including some not supported by VNR!)
- Hook text using /H "hook" codes (most AGTH codes supported)
- Directly extract text using /R "read" codes

## Support

Please let me know of any bugs, games that Textractor has trouble hooking, feature requests, or other suggestions.<br>
If you have trouble hooking a game please email me a place where I can freely download it, or gift it to me on [Steam](https://steamcommunity.com/profiles/76561198097566313/).

## Extensions

See my [Example Extension project](https://github.com/Artikash/ExampleExtension) to see how to build an extension.<br>
See the extensions folder for examples of what extensions can do. 

## Contributing

All contributions are appreciated! Please email (no, I'm not busy!) me at akashmozumdar@gmail.com if you have any questions about the codebase.<br>
You should use the standard process of making a pull request (fork, branch, commit changes, make PR from your branch to my master).<br>
Contributing a translation is easy: just translate the strings in text.cpp as well as this README.

## Compiling

Before compiling *Textractor*, you should get Visual Studio with CMake support, as well as Qt version 5.11<br>
You should then be able to simply open the folder in Visual Studio, and build. Run Textractor.exe.

## Project Architecture

The host (see GUI/host folder) injects texthook.dll (created from the texthook folder) into the target process and connects to it via 2 pipe files.<br>
Host writes to hostPipe, texthook writes to hookPipe.<br>
texthook waits for the pipe to be connected, then injects a few instructions into any text outputting functions (e.g. TextOut, GetGlyphOutline) that cause their input to be sent through the pipe.<br>
Additional information about hooks is shared through a file view (a.k.a. section object) that is mapped to a reference to the TextHook class.<br>
The text that the host receives through the pipe is then processed a little before being dispatched back to the GUI.<br>
Finally, the GUI dispatches the text to extensions before displaying it.

## Developers

If you're on this list and want your link changed let me know.
- Textractor made by [Me](https://github.com/Artikash) and [DoumanAsh](https://github.com/DoumanAsh)
- Spanish translation by [scese250](https://github.com/scese250)
- Turkish translation by niisokusu
- Simplified Chinese translation by [tinyAdapter](https://github.com/tinyAdapter)
- Russian translation by [TokcDK](https://github.com/TokcDK)
- ITHVNR updated by [mireado](https://github.com/mireado) and [Eguni](https://github.com/Eguni)
- ITHVNR originally made by [Stomp](http://www.hongfire.com/forum/member/325894-stomp)
- VNR engine made by [jichi](https://archive.is/prJwr)
- ITH updated by [Andys](https://github.com/AndyScull)
- ITH originally made by [kaosu](http://www.hongfire.com/forum/member/562651-kaosu)
- Locale Emulator library made by [xupefei](https://github.com/xupefei)
- MinHook library made by [TsudaKageyu](https://github.com/TsudaKageyu)

## Special Thanks

- Everybody adding issues!

## Contributors

This project exists thanks to all the people who contribute. 
<a href="https://github.com/Artikash/Textractor/graphs/contributors"><img src="https://opencollective.com/Textractor/contributors.svg?width=890&button=false" /></a>


## Backers

Thank you to all our backers! 🙏 [[Become a backer](https://opencollective.com/Textractor#backer)]

<a href="https://opencollective.com/Textractor#backers" target="_blank"><img src="https://opencollective.com/Textractor/backers.svg?width=890"></a>


## Sponsors

Support this project by becoming a sponsor. Your logo will show up here with a link to your website. [[Become a sponsor](https://opencollective.com/Textractor#sponsor)]

<a href="https://opencollective.com/Textractor/sponsor/0/website" target="_blank"><img src="https://opencollective.com/Textractor/sponsor/0/avatar.svg"></a>
<a href="https://opencollective.com/Textractor/sponsor/1/website" target="_blank"><img src="https://opencollective.com/Textractor/sponsor/1/avatar.svg"></a>
<a href="https://opencollective.com/Textractor/sponsor/2/website" target="_blank"><img src="https://opencollective.com/Textractor/sponsor/2/avatar.svg"></a>
<a href="https://opencollective.com/Textractor/sponsor/3/website" target="_blank"><img src="https://opencollective.com/Textractor/sponsor/3/avatar.svg"></a>
<a href="https://opencollective.com/Textractor/sponsor/4/website" target="_blank"><img src="https://opencollective.com/Textractor/sponsor/4/avatar.svg"></a>
<a href="https://opencollective.com/Textractor/sponsor/5/website" target="_blank"><img src="https://opencollective.com/Textractor/sponsor/5/avatar.svg"></a>
<a href="https://opencollective.com/Textractor/sponsor/6/website" target="_blank"><img src="https://opencollective.com/Textractor/sponsor/6/avatar.svg"></a>
<a href="https://opencollective.com/Textractor/sponsor/7/website" target="_blank"><img src="https://opencollective.com/Textractor/sponsor/7/avatar.svg"></a>
<a href="https://opencollective.com/Textractor/sponsor/8/website" target="_blank"><img src="https://opencollective.com/Textractor/sponsor/8/avatar.svg"></a>
<a href="https://opencollective.com/Textractor/sponsor/9/website" target="_blank"><img src="https://opencollective.com/Textractor/sponsor/9/avatar.svg"></a>


