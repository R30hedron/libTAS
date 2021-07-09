---
layout: page
title: Ram Watch and Search
Permalink: /guides/ramsearch/
---

* TOC
{:toc}

## Introduction

This guide is meant to summarize and give a brief overview of libTAS’s Ram Search and Ram Watch features as well as provide a more advanced workflow for finding pointers using Cheat Engine.

## Pointer Scanning using libTAS’s Ram Search and Ram Watch

### Ram Search

To find a particular memory address of interest within libTAS, we can utilize:

* Tools > Ram Search...

From here, you can use the Compare To, Comparison Operator, and Format options to specify the type of memory value you are searching for. To start a new search using the given settings, press New, and to restrict the current list using the given settings, press Search. Add Watch will send the selected address and value to the Ram Watch window.

#### Important Considerations

With the current release of libTAS (1.4.2), the following considerations should be kept in mind when using Ram Search:

* It is best to leave the Included Memory Regions with the default settings (Heap and Anon RW Mapping).
* If you are hunting for a double value, Ram Search struggles to find exact values when using the Equal To comparison operator. You will have better luck if you use Less Than and Greater Than to set lower and upper bounds.

### Ram Watch and Pointer Scanning

Once a memory address has been found, we now need to have a way to find this address when restarting the game. Most modern (and many older) games utilize pointers to allocate game memory, and as such, just using the memory address we found earlier will be insufficient. To open the Ram Watch:

* Tools > Ram Watch…

From here, select the address we found with the Ram Search and select Scan Pointer. Use the default settings and select Search. If one of the options in the list has a base address that does not include the phrase `[stack]-`, then the listed pointer result may be sufficient to use. Select it, and press Add Watch to add it to the list. You can then use Save Watches to save the current list.

If you do not find an adequate option, increase the Max level or Max offset options until one is found. Depending on your PC, increasing these options too far (Max level beyond 8 or Max offset beyond 2048) may appear to hang, and as such, is not recommended beyond these points.


## Pointer Scanning using Cheat Engine

With some games, libTAS’s Ram Search may fail to adequately find a stable pointer for the memory address you are looking for, such as if there may be too many offsets needed to find a fixed base address, or if the base address may use a library instead of the game binary. For this, it can help to use Cheat Engine ([Download Page](https://www.cheatengine.org/downloads.php))) with its more robust debugging tools. For most games, this would be my recommended method for finding memory addresses.

### Prerequisites

Because Cheat Engine is a Windows debugging tool, we will need to account for a few things to allow us to debug Linux programs. Importantly, regardless of our workflow, we will need CE Server:
* [Direct Download from above Download Page](https://www.cheatengine.org/download/ceserver_linux_x86_64.zip)
* [Github Repo](https://github.com/cheat-engine/cheat-engine/tree/master/Cheat%20Engine/ceserver)
This program will allow Cheat Engine to listen to the memory of a Linux program. The other half of this setup is Cheat Engine itself, which will depend on how your libTAS workflow has been set up.

If you are running libTAS on a native installation of Linux, the recommended option is to run Cheat Engine under [Wine](https://www.winehq.org/).
If you are running libTAS within the [Windows Subsystem for Linux (WSL 2)](https://clementgallet.github.io/libTAS/guides/wsl/), you can run Cheat Engine from the host Windows system.

### Setting up CE to Watch Linux Ram

After starting your game (either within libTAS or running natively), first start up CE Server:

    ./ceserver

Next, we need to use Cheat Engine’s network scanning to find the Linux process:

* File > Open Process
* Network
* Set Host to `localhost` and select Connect
* Select the process from the given list

From here, you now can use Cheat Engine as you would normally to find the pointers. [This forum thread](https://forum.cheatengine.org/viewtopic.php?t=602561) explains the most straightforward method, using Cheat Engine’s pointerscan feature to find your memory address of interest.

### Translating Pointers to libTAS Ram Watch

Once the memory address of interest has been found, we now can copy the pointer offsets over to libTAS’s Ram Watch. However, in doing so, we need to be aware of the differences between how the two programs handle the base address.

Within Cheat Engine, perform the following:

* Note down the base address for the pointer
* Table > Show Cheat Table Lua Script
* Enter `print(string.format('%x', getAddress("game")))`, replacing `”game”` with the module used in the base address.
* Execute Script
* Note down the value printed out by the Lua Engine.
* Add the values (in hex) from the base address and the value printed out by the Lua Engine.

This value will be the value used for the base address within libTAS’s Ram Watch.
As an example, let’s say the base address for the pointer in Cheat Engine is `”game”+00000400` and the value printed by the Lua Engine is 100000. Adding these together, the value will be 00100400. Thus, within libTAS’s Ram Watch, we will enter `game+0x00100400` as the base address. All other offsets can then be entered in hex from Cheat Engine. If Poke Value returns “Poking succeeded”, and the memory address value from libTAS’s Ram Watch matches the value in Cheat Engine, you are done.
