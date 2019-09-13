#!/usr/bin/env python2

import zipfile
import sys
import re

"""
TODO :  
- support of cps2 and cps3 roms
"""

def convert_audio_samples_cps2(name, zf, filelist, cps2_info, offset):
    data = cps2_info[offset:]
    end = data.find("key")
    pat = re.compile("ROM_LOAD16_WORD_SWAP\( \"(.*?)\"")
    res = pat.findall(data[:end])

    nb_samples_files = len(res)

    data = []
    for k in xrange(nb_samples_files):

        file1 = zf.read(res[k])
        for i in xrange(0, len(file1), 2):
            data.append(file1[i+1])
            data.append(file1[i])
        k += 1

    data = "".join(data)
    open('rom.qs', 'wb').write(data)

    return 0

def convert_audio_samples_cps1(name, zf, filelist, cps1_info, offset):
    data = cps1_info[offset:]
    end = data.find("aboard")
    pat = re.compile("ROM_LOAD\( \"(.*?)\"")
    res = pat.findall(data[:end])
    
    nb_samples_files = len(res)
     
    data = ""
    for i in xrange(nb_samples_files):
        data += zf.read(res[i])
    open('rom.oki', 'wb').write(data)
    
    return 0 

def convert_audio_cps2(name, zf, filelist, cps2_info, offset):
    data = cps2_info[offset:]
    end = data.find("qsound")
    pat = re.compile("ROM_LOAD\( \"(.*?)\"")
    res = pat.findall(data[:end])

    nb_samples_files = len(res)

    data = ""
    for i in xrange(nb_samples_files):
        data += zf.read(res[i])
    open('rom.z80', 'wb').write(data)
    return offset+end

def convert_audio_cps1(name, zf, filelist, cps1_info, offset):
    data = cps1_info[offset:]
    end = data.find("oki")
    pat = re.compile("ROM_LOAD\( \"(.*?)\"")
    res = pat.findall(data[:end])
  
    data = zf.read(res[0])
    open('rom.z80', 'wb').write(data)
    return offset+end 
    
def convert_maincpu(name, zf, filelist, cps1_info):
    cpu_files = []
    s = "ROM_START( " + name + " )"
    begin = cps1_info.find(s)
    data = cps1_info[begin:]
    end = data.find("gfx")
    pat = re.compile("ROM_LOAD16_(.*?),")
    res = pat.findall(data[:end])
    nb_cpu_files = len(res)
    
    for n in xrange(nb_cpu_files):
        filename = re.search('\"(.*)\"', res[n]).group(0)[1:-1]
        cpu_files.append(filename) 
     
    maincpu = []
    k = 0
    while k < len(cpu_files):
        format = re.search('(.*)\(', res[k]).group(0)[:-1]
       
        # ROM_LOAD16_BYTE
        if format == "BYTE":
            file1 = zf.read(cpu_files[k])
            file2 = zf.read(cpu_files[k+1])
        
            for i in xrange(len(file1)):
                maincpu.append(file1[i])
                maincpu.append(file2[i])
            k += 2
        
        # ROM_LOAD16_WORD_SWAP
        elif format == "WORD_SWAP":
            file1 = zf.read(cpu_files[k])
            for i in xrange(0, len(file1), 2):
                maincpu.append(file1[i+1])
                maincpu.append(file1[i])
            k += 1
            
        # ROM_LOAD16_WORD
        elif format == "WORD":
            file1 = zf.read(cpu_files[k])
            maincpu.append(file1)
            k += 1
             
    maincpu = "".join(maincpu)        
    open('rom.68k', 'wb').write(maincpu)
   
    return begin+end 

def unshuffle(idx, length):
    global cps_gfx
    if length == 2:
        return
    if length %4 != 0:
        print "ERROR:  This should not happen!"
        return

    length /= 2

    unshuffle(idx, length)
    unshuffle(idx + length, length)

    for i in xrange(length/2):
        t = cps_gfx[idx + length / 2 + i]
        cps_gfx[idx + length / 2 + i] = cps_gfx[idx + length + i]
        cps_gfx[idx + length + i] = t
  
def convert_gfx(name, zf, filelist, cps_info, machine, offset):
    global cps_gfx
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
    for i in xrange(nb_gfx_files):
        vrom_filesize += len(zf.read(res[i]))
    
    # fill vrom array
    cps_gfx = ['\x00'] * vrom_filesize
    l = -1
    length = 0
    prev_length = 0
    for m in xrange(0, nb_gfx_files, 4):
        l+=1
        prev_length += length
        for n in xrange(0, 8, 2):
           data = zf.read(res[(n/2)+m])
           length = len(data) * 4
           if m == 0:
            ptr = length * l
           else:
            # length of previous block
            ptr = prev_length
           j = 0
           for i in xrange(0, length, 8):
            cps_gfx[i+n+ptr] = data[j]
            cps_gfx[i+n+ptr+1] = data[j+1]
            j+=2
           

    gfxsize = vrom_filesize / 4

    if machine == "CPS2":
        banksize = 0x200000
        for i in xrange(0, vrom_filesize, banksize):
            unshuffle(i, banksize/8)

    # decode gfx 
    for i in xrange(gfxsize):
        src = ord(cps_gfx[4 * i]) + (ord(cps_gfx[4 * i + 1]) << 8) + (ord(cps_gfx[4 * i + 2]) << 16) + (ord(cps_gfx[4 * i + 3]) << 24)
        dwval = 0

        for j in xrange(8):
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
        
        cps_gfx[4 *i    ] = chr((dwval >> 0) & 0xff)
        cps_gfx[4 *i + 1] = chr((dwval >> 8) & 0xff)
        cps_gfx[4 *i + 2] = chr((dwval >> 16) & 0xff)
        cps_gfx[4 *i + 3] = chr((dwval >> 24) & 0xff)
     
    # write vrom file
    out = "".join(cps_gfx)
    open('rom.vrom', 'wb').write(out)
    return offset+end

def convert_cps2(name, zf, filelist, cps_info):
    print "Converting maincpu..."
    offset = convert_maincpu(name, zf, filelist, cps_info)
    print "[+] maincpu converted"

    print "Converting gfx..."
    offset = convert_gfx(name, zf, filelist, cps_info, "CPS2", offset)
    print "[+] gfx converted"

    print "Converting audio..."
    offset = convert_audio_cps2(name, zf, filelist, cps_info, offset)
    print "[+] audio converted"

    print "Converting qsound samples.."
    convert_audio_samples_cps2(name, zf, filelist, cps_info, offset)
    print "[+] qsound samples extracted"

def convert_cps1(name, zf, filelist, cps_info):
    print "Converting maincpu..."
    offset = convert_maincpu(name, zf, filelist, cps_info)
    print "[+] maincpu converted"
  
    print "Converting gfx..."
    offset = convert_gfx(name, zf, filelist, cps_info, "CPS1", offset)
    print "[+] gfx converted"
    
    print "Converting audio..."
    offset = convert_audio_cps1(name, zf, filelist, cps_info, offset)
    print "[+] audio converted"
    
    print "Converting audio samples..."
    convert_audio_samples_cps1(name, zf, filelist, cps_info, offset)
    print "[+] audio samples extracted"

def usage():
    print "Usage : %s [romfile] [machine]" % sys.argv[0]
    sys.exit(0)
    
def main(argc, argv):
    if argc != 3:
        usage()
        
    zipped_rom = argv[1]
    zf = zipfile.ZipFile(zipped_rom)
    filelist = zf.infolist()
    if argv[2] == "cps1":
        cps_info = open('cps1.cpp', 'r').read()
        convert_cps1(zipped_rom[:-4], zf, filelist, cps_info)
    if argv[2] == "cps2":
        cps_info = open('cps2.cpp', 'r').read()
        convert_cps2(zipped_rom[:-4], zf, filelist, cps_info)
    elif argv[2] == "cps3":
        print "[-] not suppported yet"
        sys.exit(0)
    else:
        usage()
    
if __name__ == "__main__":
    cps_gfx = []
    main(len(sys.argv), sys.argv)
