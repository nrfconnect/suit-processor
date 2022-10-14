#!/usr/bin/env python

import argparse

def dump_cfile(in_file, out_file, arr_name):
    with open(in_file, 'rb') as in_file:
        with open(out_file, 'w') as out_file:
            out_file.write(f"static uint8_t {arr_name}[] = ""{\n")
            i = 0
            for byte in in_file.read():
                if i % 8 == 0:
                    out_file.write("\t")
                else:
                    out_file.write(" ")
                out_file.write(f"0x{byte:02X},")
                if i % 8 == 7:
                    out_file.write("\n")
                i += 1
            if i % 8 != 0:
                out_file.write("\n")
            out_file.write("};\n")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate C-style array for a binary file')
    parser.add_argument('input', metavar='i', type=str, help='path to the input binary file')
    parser.add_argument('--output', metavar='o', type=str, help='path to the output C source file')
    parser.add_argument('--name', metavar='n', type=str, default="manifest_buf", help='variable name ')

    args = parser.parse_args()
    if args.output is None:
        output_path = args.input + ".c"
    else:
        output_path = args.output
    dump_cfile(args.input, output_path, args.name)
