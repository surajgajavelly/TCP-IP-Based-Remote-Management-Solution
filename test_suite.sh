#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${GREEN}--- Starting TCP Remote Diagnostic Test Suite ---${NC}"

# 1. Build
make clean && make

# 2. Start Server
./bin/server &
SERVER_PID=$!
sleep 1 

# 3. Run automated Client tests (Using 'bash' as the target)
echo "Step 4: Running automated Client tests..."
./bin/client 127.0.0.1 <<EOF
help
get-mem bash
get-open-fd bash
get-cpu-usage bash
get-ports-used server
history
exit
EOF

# 4. Cleanup
echo -e "\n${GREEN}Step 5: Cleaning up...${NC}"
kill $SERVER_PID
echo -e "${GREEN}Test Suite Completed.${NC}"