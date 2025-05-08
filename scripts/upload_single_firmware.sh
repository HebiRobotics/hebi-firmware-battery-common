#!/bin/bash

# A script to upload a single firmware directly to the firmware server

if [ $# != 7 ]
then
  echo "Usage: ./upload_single_firmware.sh <FW type (BETA/STABLE/etc)> <electrical type> <source file name> <dest file name> <upload label> <upload token> <url>"
  exit 1
fi

RES=$(curl -X POST -w "%{http_code}" -o res.txt \
  -H "Token: $6" \
  -H "Accept: application/json" \
  -H "content-type: multipart/form-data" \
  -F "module_type=$1" \
  -F "electrical_type=$2" \
  -F "firmware_type=APPLICATION" \
  -F "label=$5" \
  -F "file_name=$4" \
  -F "file=@$3;type=application/octet-stream" \
  $7)

# Check for status code; this fails if http status != 200
# NOTE: could also check "res.txt" to ensure it has returned an ID;
# the expected response format right now is {"id"=340} (where the
# actual number will vary per upload)
if [ ${RES} != 200 ]
then
  echo "Upload script did not match expected HTTP return code!"
  exit 1
fi
