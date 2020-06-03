#!/usr/bin/env python3

import zipfile
import sys
import re
import os
import subprocess

cps2_keys = {}
cps2_keys["sfa2ur1"] = "0x1bbf3d96 0x8af4614a"
cps2_keys["vsavj"] = "0xfa8f4e33 0xa4b881b9"
cps2_keys["xmcotaj"] = "0x46027315 0xaf8bcd9e"
cps2_keys["sfa3u"] = "0xe7bbf0e5 0x67943248"
cps2_keys["ringdest"] = "0x19940727 0x17444903"
cps2_keys["msh"] = "0x1a11ee26 0xe7955d17"
cps2_keys["mvsc"] = "0x48025ade 0x1c697b27"
cps2_keys["dstlk"] = "0x13d8a7a8 0x0008b090"
cps2_keys["megamn2d"] = "0x50501cac 0xed346550"
cps2_keys["1944"] = "0x1d3e724c 0x8b59fc7a"
cps2_keys["hsf2"] = "5a369ddd fea3189c"
cps2_keys["ssf2u"] = "0x12345678 0x9abcdef0"
cps2_keys["ssf2t"] = "0x944e8302 0x56d3143c"
cps2_keys["spf2t"] = "0x706a8750 0x7d0fc185"


def convert_audio_samples_cps2(name, zf, filelist, cps_info, offset):
    data = cps_info[offset:]
    end = data.find("key")
    pat = re.compile("ROM_LOAD16_WORD_SWAP\( \"(.*?)\"")
    res = pat.findall(data[:end])

    nb_samples_files = len(res)

    qsound = bytearray()
    for k in range(nb_samples_files):

        file1 = zf.read(res[k])
        for i in range(0, len(file1), 2):
            qsound.append(file1[i+1])
            qsound.append(file1[i])
        k += 1

    open('rom.qs', 'wb').write(qsound)

    return 0

def convert_audio_samples_cps1(name, zf, filelist, cps_info, offset):
    data = cps_info[offset:]
    end = data.find("aboard")
    pat = re.compile("ROM_LOAD\( \"(.*?)\"")
    res = pat.findall(data[:end])

    nb_samples_files = len(res)

    oki = bytearray()
    for i in range(nb_samples_files):
        oki += zf.read(res[i])
    open('rom.oki', 'wb').write(oki)

    return 0

def convert_audio_cps2(name, zf, filelist, cps2_info, offset):
    data = cps2_info[offset:]
    end = data.find("qsound")
    pat = re.compile("ROM_LOAD\( \"(.*?)\"")
    res = pat.findall(data[:end])

    nb_samples_files = len(res)

    z80 = bytearray()
    for i in range(nb_samples_files):
        z80 += zf.read(res[i])
    open('rom.z80', 'wb').write(z80)

    return offset+end

def convert_audio_cps1(name, zf, filelist, cps_info, offset):
    data = cps_info[offset:]
    end = data.find("oki")
    pat = re.compile("ROM_LOAD\( \"(.*?)\"")
    res = pat.findall(data[:end])

    z80 = zf.read(res[0])
    open('rom.z80', 'wb').write(z80)

    return offset+end

def decrypt_maincpu(name, zf, filelist, cps_info):
    cpu_files = []
    s = "ROM_START( " + name
    begin = cps_info.find(s)
    data = cps_info[begin:]
    end = data.find("gfx")
    pat = re.compile("ROM_LOAD16_(.*?),")
    res = pat.findall(data[:end])
    nb_cpu_files = len(res)
    data = ""
    
    # concat all sample files
    for n in range(nb_cpu_files):
        filename = re.search('\"(.*)\"', res[n]).group(0)[1:-1]
        print(filename)
        data += zf.read(filename)
    open('rom.tmp', 'wb').write(data)
    f = open('rom.68y', 'w')
    print(name)
    subprocess.Popen(["rahash2", "-D", "cps2", "-S", cps2_keys[name], "rom.tmp"], stdout=f)
    
def convert_maincpu(name, zf, filelist, cps_info):
    cpu_files = []
    s = "ROM_START( " + name + " )"
    begin = cps_info.find(s)
    data = cps_info[begin:]
    end = data.find("gfx")
    pat = re.compile("ROM_LOAD16_(.*?),")
    res = pat.findall(data[:end])
    nb_cpu_files = len(res)

    for n in range(nb_cpu_files):
        filename = re.search('\"(.*)\"', res[n]).group(0)[1:-1]
        cpu_files.append(filename)

    maincpu = bytearray()
    k = 0
    while k < len(cpu_files):
        format = re.search('(.*)\(', res[k]).group(0)[:-1]

        # ROM_LOAD16_BYTE
        if format == "BYTE":
            file1 = zf.read(cpu_files[k])
            file2 = zf.read(cpu_files[k+1])

            for i in range(len(file1)):
                maincpu.append(file1[i])
                maincpu.append(file2[i])
            k += 2

        # ROM_LOAD16_WORD_SWAP
        elif format == "WORD_SWAP":
            file1 = zf.read(cpu_files[k])
            for i in range(0, len(file1), 2):
                maincpu.append(file1[i+1])
                maincpu.append(file1[i])
            k += 1

        # ROM_LOAD16_WORD
        elif format == "WORD":
            file1 = zf.read(cpu_files[k])
            maincpu.append(file1)
            k += 1

    open('rom.68k', 'wb').write(maincpu)

    return begin+end

def convert_bios(name, zf, filelist, cps_info):
    s = "ROM_START( " + name + " )"
    begin = cps_info.find(s)
    data = cps_info[begin:]
    end = data.find("simm")
    pat = re.compile("ROM_LOAD\( \"(.*?)\"")
    res = pat.findall(data[:end])

    bios = zf.read(res[0])
    open('rom.bios', 'wb').write(bios)

    return begin+end

def convert_simm(name, zf, filelist, cps_info, offset):
    data = cps_info[offset:]
    end = data.find("ROM_END")
    pat = re.compile("ROM_LOAD\( \"(.*?)\"")
    res = pat.findall(data[:end])

    for k in range(1,7):
        simm = bytearray()
        pat = re.compile(r".*simm%d\..*" % k)
        simm_files = list(filter(pat.match, res))
        nb_simm_files = len(simm_files)
        if simm_files:
            for i in range(nb_simm_files):
                simm += zf.read(simm_files[i])
            open('rom.s%d' % k, 'wb').write(simm)

    return 0

def unshuffle(cps_gfx, idx, length):
    if length == 2:
        return
    if length %4 != 0:
        print("ERROR:  This should not happen!")
        return

    length //= 2

    unshuffle(cps_gfx, idx, length)
    unshuffle(cps_gfx, idx + (length * 8), length)

    for i in range(length//2):
        pos1 = 8 * (i + length // 2) + idx
        pos2 = 8 * (i + length) + idx
        cps_gfx[pos1: pos1 + 8], cps_gfx[pos2: pos2 + 8] = \
            cps_gfx[pos2: pos2 + 8], cps_gfx[pos1: pos1 + 8]

def convert_gfx(name, zf, filelist, cps_info, offset, machine):
    gfx_files = []
    data = cps_info[offset:]
    end = data.find("audiocpu")
    if machine == "CPS1":
        pat = re.compile("ROMX_LOAD\( \"(.*?)\"")
    elif machine == "CPS2":
        pat = re.compile("ROM_LOAD64_WORD\( \"(.*?)\"")
    res = pat.findall(data[:end])
    nb_gfx_files = len(res)

    # calculate vrom filesize
    vrom_filesize = 0
    for i in range(nb_gfx_files):
        vrom_filesize += len(zf.read(res[i]))

    cps_gfx = bytearray(vrom_filesize)
    l = -1
    length = 0
    prev_length = 0
    for m in range(0, nb_gfx_files, 4):
        l+=1
        prev_length += length
        for n in range(0, 8, 2):
           data = zf.read(res[(n//2)+m])
           length = len(data) * 4
           if m == 0:
            ptr = length * l
           else:
            # length of previous block
            ptr = prev_length
           j = 0
           for i in range(0, length, 8):
            cps_gfx[i+n+ptr] = data[j]
            cps_gfx[i+n+ptr+1] = data[j+1]
            j+=2

    if machine == "CPS2":
        banksize = 0x200000
        for i in range(0, vrom_filesize, banksize):
            unshuffle(cps_gfx, i, banksize//8)

    # decode gfx
    for i in range(0, vrom_filesize, 4):
        src = cps_gfx[i] + (cps_gfx[i + 1] << 8) + (cps_gfx[i + 2] << 16) + (cps_gfx[i + 3] << 24)
        dwval = 0

        for j in range(8):
            n = 0
            mask = (0x80808080 >> j) & src

            if (mask & 0x000000ff):
                n |= 1
            if (mask & 0x0000ff00):
                n |= 2
            if (mask & 0x00ff0000):
                n |= 4
            if (mask & 0xff000000):
                n |= 8

            dwval |= n << (j * 4)

        cps_gfx[i    ] = (dwval >> 0) & 0xff
        cps_gfx[i + 1] = (dwval >> 8) & 0xff
        cps_gfx[i + 2] = (dwval >> 16) & 0xff
        cps_gfx[i + 3] = (dwval >> 24) & 0xff

    # write vrom file
    open('rom.vrom', 'wb').write(cps_gfx)

    return offset+end

def convert_cps3(name, zf, filelist, cps_info):
    print("Converting BIOS...")
    offset = convert_bios(name, zf, filelist, cps_info)
    print("[+] BIOS converted")
    convert_simm(name, zf, filelist, cps_info, offset)
    print("Converting game data")
    print("[+] game data converted")

def convert_cps2(name, zf, filelist, cps_info):
    print("Decrypting maincpu...")
    decrypt_maincpu(name, zf, filelist, cps_info)
    print("[+] maincpu decrypted")
    
    print("Converting maincpu...")
    offset = convert_maincpu(name, zf, filelist, cps_info)
    print("[+] maincpu converted")

    print("Converting gfx...")
    offset = convert_gfx(name, zf, filelist, cps_info, offset, "CPS2")
    print("[+] gfx converted")

    print("Converting audio...")
    offset = convert_audio_cps2(name, zf, filelist, cps_info, offset)
    print("[+] audio converted")

    print("Converting qsound samples..")
    convert_audio_samples_cps2(name, zf, filelist, cps_info, offset)
    print("[+] qsound samples extracted")
    
    os.remove('rom.tmp')

def convert_cps1(name, zf, filelist, cps_info):
    print("Converting maincpu...")
    offset = convert_maincpu(name, zf, filelist, cps_info)
    print("[+] maincpu converted")

    print("Converting gfx...")
    offset = convert_gfx(name, zf, filelist, cps_info, offset, "CPS1")
    print("[+] gfx converted")

    print("Converting audio...")
    offset = convert_audio_cps1(name, zf, filelist, cps_info, offset)
    print("[+] audio converted")

    print("Converting audio samples...")
    convert_audio_samples_cps1(name, zf, filelist, cps_info, offset)
    print("[+] audio samples extracted")

def usage():
    print("Usage : %s [romfile]" % sys.argv[0])
    sys.exit(0)

def main(argc, argv):
    if argc != 2:
        usage()

    zipped_rom = argv[1]
    zf = zipfile.ZipFile(zipped_rom)
    filelist = zf.infolist()
    name = zipped_rom[:-4]

    for i in range(1,4):
        cps_info = open('cps%d.cpp' % i, 'r').read()
        s = "ROM_START( " + name + " )"
        if cps_info.find(s) > 0:
           machine = "CPS%d" % i
    if machine == "CPS1":
        cps_info = open('cps1.cpp', 'r').read()
        convert_cps1(name, zf, filelist, cps_info)
    elif machine == "CPS2":
        cps_info = open('cps2.cpp', 'r').read()
        convert_cps2(name, zf, filelist, cps_info)
    elif machine == "CPS3":
        cps_info = open('cps3.cpp', 'r').read()
        convert_cps3(name, zf, filelist, cps_info)
    else:
        print("Could not find %s in cps info files." % name)

if __name__ == "__main__":
    main(len(sys.argv), sys.argv)
