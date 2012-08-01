#!/bin/sh

# Usage: from the tests subdirectory:
#    ./generate_bescmd.sh filename responseType outputFilename [constraint_expr]
# 
# Generates an XML bes command file that can be loaded with -i 
# Useful for creating test files for debugging.
# 
# @author mjohnson <m.johnson@opendap.org>

# input to process to make $BESCMD_FILENAME
TEMPLATE_FILENAME="template.bescmd.in"

DATA_FILENAME=$1
RESPONSE=$2
OUTPUT_FILENAME=$3
CONSTRAINT_EXPR=$4

echo "$0: Generating $OUTPUT_FILENAME for $DATA_FILENAME response=$RESPONSE constraint=\"$CONSTRAINT_EXPR\""
if test -n "$CONSTRAINT_EXPR"
then 
echo "$0: Using constraint=\"$CONSTRAINT_EXPR\""
sed -e "s|%filename%|$1|" -e "s:%response_type%:$RESPONSE:" -e "s|%constraint_expr%|<constraint>$CONSTRAINT_EXPR</constraint>|" < $TEMPLATE_FILENAME > $OUTPUT_FILENAME;
else 
echo "$0: No constraint."
sed -e "s|%filename%|$1|" -e "s|%response_type%|$RESPONSE|" -e "s|%constraint_expr%||" < $TEMPLATE_FILENAME > $OUTPUT_FILENAME; 
fi


