#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#define check_nbit(X,n) ((X >> n) & 1)
#define datashift1(X) ((check_nbit(X,4) ? 0b00001 : 0b0) | (check_nbit(X,3) ? 0b10000 : 0b0)|(check_nbit(X,2) ? 0b01000 : 0b0)|(check_nbit(X,1) ? 0b00100 : 0b0)|(check_nbit(X,0) ? 0b00010 : 0b0))
#define datashift2(X) ((check_nbit(X,4) ? 0b00010 : 0b0) | (check_nbit(X,3) ? 0b00001 : 0b0)|(check_nbit(X,2) ? 0b10000 : 0b0)|(check_nbit(X,1) ? 0b01000 : 0b0)|(check_nbit(X,0) ? 0b00100 : 0b0))
#define swbit(data) ((data & 0b11110000) >> 4 | (data & 0b00001111) << 4)
#define bitmerge(top, bottom) (top << 4 | bottom)

int s0[4][4] = { 
	{1,0,3,2},
	{3,2,1,0},
	{0,2,1,3},
	{3,1,3,2}
};

int s1[4][4] = {
	{0,1,2,3},
	{2,0,1,3},
	{3,0,1,0},
	{2,1,0,3}
};

int ip8index[] = { 2,6,3,1,4,8,5,7 };
int ip8_1index[] = { 4,1,3,5,7,2,8,6 };
int epindex[] = { 4,1,2,3,2,3,4,1 };
int p4index[] = { 2,4,3,1 };
int p8_top = 0b11110000;
int p8_bottom = 0b00001111;

void print_bin(FILE *fp, int data) {
	int i;
	for (i = 9; i >= 0; i--) {
		fprintf(fp, "%d", check_nbit(data,i));
	}
	fprintf(fp, "\n");
}

int substitution_10(int input, int arr[]) {
	int result = 0b0;
	int i;
	for (i = 0; i < 10; i++) {		
		result |= (check_nbit(input, 10 - arr[i]) << (9 - i));
	}
	return result;
}
//10bit data substitution


int key_substitution_8(int input, int arr[]) {
	int result = 0b0;
	int i;
	for (i = 0; i < 8; i++) {
		result |= (check_nbit(input, 10 - arr[i]) << (7 - i));
	}
	return result;
}
//8bit data substitution

int ip_substitution_8(int input, int arr[]) {
	int result = 0b0;
	int i;
	for (i = 0; i < 8; i++) {
		result |= (check_nbit(input, 8 - arr[i]) << (7 - i));
	}
	return result;
}

int ep_4(int input, int arr[]) {
	int result = 0b0;
	int i;
	for (i = 0; i < 8; i++) {
		result |= (check_nbit(input, 4 - arr[i]) << (7 - i));
	}
	return result;
}

int p_4(int input, int arr[]) {
	int result = 0b0;
	int i;
	for (i = 0; i < 4; i++) {
		result |= (check_nbit(input, 4 - arr[i]) << (3 - i));
	}
	return result;
}

int sbox_calc(int top_input, int bottom_input) {
	int top_i = check_nbit(top_input, 3) << 1 | check_nbit(top_input, 0);
	int top_j = check_nbit(top_input, 2) << 1 | check_nbit(top_input, 1);
	int bottom_i = check_nbit(bottom_input, 3) << 1 | check_nbit(bottom_input, 0);
	int bottom_j = check_nbit(bottom_input, 2) << 1 | check_nbit(bottom_input, 1);

	return (s0[top_i][top_j] << 2) | s1[bottom_i][bottom_j];
}


void key_generate(int data, int *subkey1, int *subkey2){
	int encode_index10[] = {3,5,2,7,4,10,1,9,8,6};
	int encode_index8[] = {6,3,7,4,8,5,10,9};

	int p10_top = 0b1111100000;
	int p10_bottom = 0b0000011111;
	int result;
	int result_top;
	int result_bottom;

	result = substitution_10(data, encode_index10);
	result_top = (p10_top & result) >> 5;
	result_bottom = p10_bottom & result;
	result_top = datashift1(result_top);
	result_bottom = datashift1(result_bottom);
	result = (result_top << 5) | result_bottom;
	*subkey1 = key_substitution_8(result, encode_index8);


	result_top = datashift2(result_top);
	result_bottom = datashift2(result_bottom);
	result = (result_top << 5) | result_bottom;
	*subkey2 = key_substitution_8(result, encode_index8);
}

int funk(int input, int subkey) {
	int ep_top_result;
	int ep_bottom_result;
	int ep_result;
	int sbox_result;
	int p_result;
	
	ep_result = ep_4(input, epindex);
	ep_result ^= subkey;
	ep_top_result = (ep_result & p8_top) >> 4;
	ep_bottom_result = ep_result & p8_bottom;
	//epbox result

	sbox_result = sbox_calc(ep_top_result, ep_bottom_result);
	//sbox result
	p_result = p_4(sbox_result, p4index);
	//sbox result permutation

	return p_result;
}

int sdes_encrypt(int data, int subkey1, int subkey2, FILE *fp){
	
	int ip_top_result;
	int ip_bottom_result;
	int ip_result;
	int sw_result;
	int sw_top;
	int sw_bottom;
	int final_result;
	
	ip_result = ip_substitution_8(data, ip8index);
	ip_top_result = (ip_result & p8_top) >> 4;
	ip_bottom_result = ip_result & p8_bottom;
	//ipbox result 
	fprintf(fp, "ip의 결과물 :");
	print_bin(fp, ip_result);

	ip_top_result ^= funk(ip_bottom_result, subkey1);
	//fk1 process complete, funk result xor top result bit

	sw_result = swbit(bitmerge(ip_top_result, ip_bottom_result));

	//top,bottom bit merge and switch bit

	sw_top = (sw_result & p8_top) >> 4;
	sw_bottom = sw_result & p8_bottom;
	
	sw_top ^= funk(sw_bottom, subkey2);
	//fk2 process complete, funk result xor top result bit

	sw_result = bitmerge(sw_top, sw_bottom);
	//top, bottom bit merge

	final_result = ip_substitution_8(sw_result, ip8_1index);
	//input ip -1 box result;

	fprintf(fp, "ip-1의 결과물 :");
	print_bin(fp, final_result);

	return final_result;
}

int main() {
	int data = 0b10010001;
	int key = 0b1011001110;
	int subkey1;
	int subkey2;
	int encode_result;
	int decode_result;

	FILE* fp = fopen("data1.dat", "w");

	fprintf(fp, "plain text ");
	print_bin(fp, data);
	fprintf(fp, "\n");

	key_generate(key, &subkey1, &subkey2);
	//key_generation

	fprintf(fp, "encryption\n");
	encode_result = sdes_encrypt(data, subkey1, subkey2, fp);
	fprintf(fp, "encryption result : ");
	print_bin(fp, encode_result);
	fprintf(fp, "\n");
	//encode data

	fprintf(fp, "decryption\n");
	decode_result = sdes_encrypt(encode_result, subkey2, subkey1, fp);
	fprintf(fp, "decryption result : ");
	print_bin(fp, decode_result);
	fprintf(fp, "\n");
	//decode data, key order switch

	if(fp != NULL)
		fclose(fp);

	return 0;
}