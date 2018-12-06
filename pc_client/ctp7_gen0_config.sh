
source ./env.sh
echo "Configure Layer-1A card"
./card_config 0 ctp7_test_algo
./layer1_config 0

echo "Configure Layer-1B card"
./card_config 1 ctp7_test_algo
./layer1_config 1

echo "Configure Layer-2 card"
./card_config 2 ctp7_test_algo
./layer2_config 2
