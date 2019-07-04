#!/usr/bin/env python2

import zipfile
import sys
import re

"""
TODO : 
- fix extract_gfx() when there are more than 4 gfx files 
- support of cps2 and cps3 roms
"""

def extract_audio_samples(name, zf, filelist, cps1_info, offset):
    data = cps1_info[offset:]
    end = data.find("aboard")
    pat = re.compile("ROM_LOAD\( \"(.*?)\"")
    res = pat.findall(data[:end])
    
    nb_samples_files = len(res)
    
    # concat all samples files 
    data = ""
    for i in xrange(nb_samples_files):
        data += zf.read(res[i])
    open('rom.oki', 'wb').write(data)
    
    return 0 

# audiocpu => name.z80 
def extract_audio(name, zf, filelist, cps1_info, offset):
    data = cps1_info[offset:]
    end = data.find("oki")
    pat = re.compile("ROM_LOAD\( \"(.*?)\"")
    res = pat.findall(data[:end])
  
    data = zf.read(res[0])
    open('rom.z80', 'wb').write(data)
    return offset+end 

def extract_cpu(name, zf, filelist, cps1_info):
    cpu_files = []
    s = "ROM_START( " + name
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
    for k in xrange(0, len(cpu_files), 2):
        
        # SWAP_WORD
        if k == len(cpu_files)-1:
            file1 = zf.read(cpu_files[k])
            for i in xrange(0, len(file1), 2):
                maincpu.append(file1[i+1])
                maincpu.append(file1[i])
            break

        file1 = zf.read(cpu_files[k])
        file2 = zf.read(cpu_files[k+1])
        
        for i in xrange(len(file1)):
            maincpu.append(file1[i])
            maincpu.append(file2[i])

    maincpu = "".join(maincpu)        
    open('rom.68k', 'wb').write(maincpu)
    return begin+end 


def extract_gfx(name, zf, filelist, cps1_info, offset):
    # parse cps1.cpp to get gfx files order
    gfx_files = []
    data = cps1_info[offset:]
    end = data.find("audiocpu")
    pat = re.compile("ROMX_LOAD\( \"(.*?)\"")
    res = pat.findall(data[:end])
    nb_gfx_files = len(res)
  
    # prepare vrom array 
    data = zf.read(res[0])
    vrom_filesize = len(data)*nb_gfx_files
    cps1_gfx = ['\x00'] * vrom_filesize

    # fill vrom array
    for n in xrange(0, nb_gfx_files*2, 2):
        data = zf.read(res[n/2])
        j = 0
        for i in xrange(0, len(cps1_gfx), nb_gfx_files*2):
            cps1_gfx[i+n] = data[j]
            cps1_gfx[i+n+1] = data[j+1]
            j+=2

    # decode gfx 
    gfxsize = vrom_filesize / 4
    for i in xrange(gfxsize):
        src = ord(cps1_gfx[4 * i]) + (ord(cps1_gfx[4 * i + 1]) << 8) + (ord(cps1_gfx[4 * i + 2]) << 16) + (ord(cps1_gfx[4 * i + 3]) << 24)
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
        
        cps1_gfx[4 *i    ] = chr((dwval >> 0) & 0xff)
        cps1_gfx[4 *i + 1] = chr((dwval >> 8) & 0xff)
        cps1_gfx[4 *i + 2] = chr((dwval >> 16) & 0xff)
        cps1_gfx[4 *i + 3] = chr((dwval >> 24) & 0xff)
   
    # write vrom file
    out = "".join(cps1_gfx)
    open('rom.vrom', 'wb').write(out)
    return offset+end

if len(sys.argv) != 2:
    print "Usage : %s [romfile]" % sys.argv[0]
    sys.exit(0)

zipped_rom = sys.argv[1]
zf = zipfile.ZipFile(zipped_rom)
filelist = zf.infolist()
cps1_info = open('cps1.cpp', 'r').read()
filelist = zf.infolist()

print "Extracting maincpu..."
offset = extract_cpu(zipped_rom[:-4], zf, filelist, cps1_info)
print "[+] maincpu extracted "

print "Extracting gfx..."
offset = extract_gfx(zipped_rom[:-4], zf, filelist, cps1_info, offset)
print "[+] gfx extracted"

print "Extracting audio..."
offset = extract_audio(zipped_rom[:-4], zf, filelist, cps1_info, offset)
print "[+] audio extracted"

print "Extracting audio samples..."
extract_audio_samples(zipped_rom[:-4], zf, filelist, cps1_info, offset)
print "[+] audio samples extracted"
