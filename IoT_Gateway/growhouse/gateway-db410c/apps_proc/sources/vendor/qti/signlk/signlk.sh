################################################################################
# Copyright (c) 2016, The Linux Foundation. All rights reserved.               #
#                                                                              #
# Redistribution and use in source and binary forms, with or without           #
# modification, are permitted provided that the following conditions are       #
# met:                                                                         #
#     * Redistributions of source code must retain the above copyright         #
#       notice, this list of conditions and the following disclaimer.          #
#     * Redistributions in binary form must reproduce the above                #
#       copyright notice, this list of conditions and the following            #
#       disclaimer in the documentation and/or other materials provided        #
#       with the distribution.                                                 #
#     * Neither the name of The Linux Foundation nor the names of its          #
#       contributors may be used to endorse or promote products derived        #
#       from this software without specific prior written permission.          #
#                                                                              #
# THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED                 #
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF         #
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT       #
# ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS       #
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR       #
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF         #
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR              #
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,        #
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE         #
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN       #
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                                #
#                                                                              #
# THIS IS A WORKAROUND TO GET LK INTO THE PROPER FORMAT,                       #
# NO SECURITY IS BEING PROVIDED BY THE OPENSSL CALLS                           #
################################################################################
INFILE=""
OUTFILE=""
DIR=$(dirname $0)
EXECUTABLE="$DIR/signer/signlk"
CN=""
OU=""
for i in "$@"
do
case $i in
    -i=*|--in=*)
    INFILE="${i#*=}"

    ;;
    -o=*|--out=*)
    OUTFILE="${i#*=}"
    ;;
    -OU=*|-ou=*)
    OU="${i#*=}"

    ;;
    -CN=*|-cn=*)
    CN="${i#*=}"

    ;;
    -h*|--help*)
    echo "signlk -i=input_file_name [-o=output_file_name]"
    echo "-i                    input ELF/MBN file name" 
    echo "-o                    output file name, input_file_name with suffix of 'signed' as default "
    echo "-cn                   common name "
    echo "-ou                   organization unit "
    exit 0
    ;;
    *)
    echo "unsupported option"       # unknown option
    echo "type signlk --help for help" 
    exit 1
    ;;
esac
done

if [ "$INFILE" == "" ]; then
   echo "signlk -i=input_file_name [-o=output_file_name]"
	exit 2
fi

INFILE_filename=$(echo $INFILE | rev | cut -f 2- -d '.' | rev)
INFILE_extension=$(echo $INFILE | rev | cut -f 1 -d '.' | rev)

if [ "$INFILE_extension" == "" ]; then
   echo "input file must be an elf or mbn file"
   exit 3      
fi

if [ "$INFILE_extension" != "elf" ] &&  [ "$INFILE_extension" != "mbn" ] && [ "$INFILE_extension" != "ELF" ] && [ "$INFILE_extension" != "MBN" ]; then
   echo "input file must be an elf or mbn file"
   exit 4
fi

if [ "$OUTFILE" == "" ]; then
        OUTFILE=$INFILE_filename"_signed.mbn"
        
fi
echo "generating output file $OUTFILE"

tmpdir=$(mktemp -d)
TMPOUTFILE=$tmpdir/"tmp.elf"
if [ ! "$(openssl version)" ]; then
   echo "please install openssl"
   exit 6
fi
if [ ! "$(make -v)" ]; then
   echo "please install gcc"
   exit 7
fi
if [ ! "$(g++ --version)" ]; then
   echo "please install g++"
   exit 8
fi

if [ ! -f "$EXECUTABLE" ]; then
   export mfile_path=$DIR/signer
   make -f $mfile_path/Makefile >/dev/null
   
   if [ "$?" != 0 ]; then
      echo " failed to build executable"
      exit 5       
   fi
fi

$EXECUTABLE $INFILE $TMPOUTFILE $tmpdir

openssl sha256 -binary $tmpdir/header > $tmpdir/data
cat $tmpdir/hash>> $tmpdir/data
openssl sha256 -binary $tmpdir/2 >> $tmpdir/data
openssl sha256 -binary $tmpdir/3 >> $tmpdir/data

openssl sha256 -binary $tmpdir/data > $tmpdir/stp0
cat $tmpdir/Si > $tmpdir/tmpDigest0
cat $tmpdir/stp0 >> $tmpdir/tmpDigest0 
openssl sha256 -binary $tmpdir/tmpDigest0 > $tmpdir/stp1
cat $tmpdir/So > $tmpdir/tmpDigest1
cat $tmpdir/stp1 >> $tmpdir/tmpDigest1 
openssl sha256 -binary $tmpdir/tmpDigest1 > $tmpdir/dataToSign.bin

DATA=$tmpdir/data
CODE=$tmpdir/dataToSign.bin
SIG=$tmpdir/sig
ATT=$tmpdir/atte.DER
ROOT=$tmpdir/root.DER

tmp_att_file=$tmpdir/e.ext

echo "authorityKeyIdentifier=keyid,issuer" >> $tmp_att_file
echo "basicConstraints=CA:FALSE,pathlen:0" >> $tmp_att_file
echo "keyUsage=digitalSignature" >> $tmp_att_file

openssl version > $tmpdir/days
openssl req -new -x509 -keyout $tmpdir/root_key.PEM -nodes -newkey rsa:2048 -days 7300 -set_serial 1 -sha256 -subj "/CN=$CN/O=$OU/C=/CN=DRAGONBOARD TEST PKI – NOT SECURE/L=/O=S/ST=/OU=01 0000000000000009 SW_ID/OU=02 0000000000000000 HW_ID" > $tmpdir/root_certificate.PEM   2>/dev/null
openssl x509 -in $tmpdir/root_certificate.PEM -inform PEM -outform DER > $ROOT 2>/dev/null
openssl genpkey -algorithm RSA -outform PEM -pkeyopt rsa_keygen_bits:2048 -pkeyopt rsa_keygen_pubexp:3 >$tmpdir/atte_key.PEM  2>/dev/null
openssl req -new -key $tmpdir/atte_key.PEM -subj "/CN=$CN/O=$OU/C=/CN=DRAGONBOARD TEST PKI – NOT SECURE/L=/O=/ST=/OU=01 0000000000000009 SW_ID/OU=02 0000000000000000 HW_ID" -days 7300 > $tmpdir/atte_csr.PEM  2>/dev/null
openssl x509 -req -in $tmpdir/atte_csr.PEM -CAkey $tmpdir/root_key.PEM -CA $tmpdir/root_certificate.PEM -days 7300 -set_serial 1 -extfile $tmp_att_file -sha256 -out $tmpdir/atte_cert.PEM 2>/dev/null
openssl x509 -in $tmpdir/atte_cert.PEM -inform PEM -outform DER > $ATT 2>/dev/null
openssl pkeyutl -sign -inkey $tmpdir/atte_key.PEM -in $CODE > $SIG 2>/dev/null


data_file_size=$( cut -d ' ' -f 1 <<< $(du -b $DATA) )
code_file_size=$( cut -d ' ' -f 1 <<< $(du -b $CODE) )
sig_file_size=$( cut -d ' ' -f 1 <<< $(du -b $SIG) )
atte_file_size=$( cut -d ' ' -f 1 <<< $(du -b $ATT) )
root_file_size=$( cut -d ' ' -f 1 <<< $(du -b $ROOT) )
hash_seg_file_size=$( cut -d ' ' -f 1 <<< $(du -b $tmpdir/hashSeg) )
hash_seg_offset=4096
mi_hdr_size=40

dd if=$TMPOUTFILE of=$OUTFILE  2>/dev/null
dd if=$DATA of=$OUTFILE bs=1 count=$data_file_size  seek=$[hash_seg_offset+$mi_hdr_size] 2>/dev/null
dd if=$SIG of=$OUTFILE count=$sig_file_size bs=1 seek=$[$hash_seg_offset+$mi_hdr_size+$data_file_size] 2>/dev/null
dd if=$ATT of=$OUTFILE count=$atte_file_size bs=1 seek=$[$hash_seg_offset+$mi_hdr_size+$data_file_size+$sig_file_size] 2>/dev/null
dd if=$ROOT of=$OUTFILE count=$root_file_size bs=1 seek=$[$hash_seg_offset+$mi_hdr_size+$data_file_size+$sig_file_size+$atte_file_size] 2>/dev/null
dd if=$tmpdir/hashSeg of=$OUTFILE bs=1 skip=$[$mi_hdr_size+$data_file_size+$sig_file_size+$atte_file_size+$root_file_size] seek=$[$hash_seg_offset+$mi_hdr_size+$data_file_size+$sig_file_size+$atte_file_size+$root_file_size] 2>/dev/null
dd if=$TMPOUTFILE of=$OUTFILE bs=1 skip=$[$hash_seg_offset+$hash_seg_file_size] seek=$[$hash_seg_offset+$hash_seg_file_size] 2>/dev/null

rm -rf $tmpdir

echo done
