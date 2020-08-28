#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>

#define SWAP(x, y) x^=y, y^=x, x^=y
#define MODMULTY(x, y, modn) (((x % modn) * (y % modn)) % modn)

FILE* fp;

typedef struct pbk {
	int e;
	int n;
};

typedef struct pk {
	int d;
	int n;
};

int extend_euclid(int eulern, int e) {
	int r;
	int q;
	int s;
	int t;
	int s1 = 1;
	int s2 = 0;
	int t1 = 0;
	int t2 = 1;
	int tmp = eulern;

	while (e)
	{
		q = eulern / e;
		r = (eulern % e);
		s = s1 - q * s2;
		t = t1 - q * t2;

		eulern = e;
		e = r;
		s1 = s2;
		s2 = s;
		t1 = t2;
		t2 = t;

	}
	if (eulern == 1)
	{
		if (t1 < 0)
			t1 += tmp;
		return t1;
	}
	return 0;
}
//extend euclid algorithm

int moduler_pow(int input, int pown, int modn){
	int i;
	int arr[64] = {0,};
	int result = 1;
	int sumn = pown;
	int pownum = 0;
	arr[0] = input % modn;
	for (i = 1; (int)pow(2, i) <= pown; i++) {
		arr[i] = (int)pow(arr[i-1], 2) % modn;
	}
	//pre caluate input pow n;

	i--; //index last pointer
	
	fprintf(fp,"pownum devide %d\n", pown);
	for (i; i >= 0; i--) {
		pownum = (int)pow(2, i);
		if (sumn >= pownum) {
			result = MODMULTY(result, arr[i], modn);
			sumn -= pownum;

			fprintf(fp,"%d ", pownum);
			// moduler number divide, and multiple;
		}
	}
	fprintf(fp,"\n");
	
	

	return result;
}

int encrypt(int data, pbk pbk_input){
	int result = moduler_pow(data, pbk_input.e, pbk_input.n);
	return result;
}

int decrypt(int crypto, pk pk_input){
	int result = moduler_pow(crypto, pk_input.d, pk_input.n);
	return result;
}

int main() {
	int data = 119;
	int p = 31;
	int q = 37;
	int e = 257;
	int n = p * q;
	int eulern = (p - 1) * (q - 1);
	fp = fopen("data1.dat", "w");

	pbk publickey;
	publickey.e = e;
	publickey.n = n;
	pk privatekey;
	privatekey.n = n;
	privatekey.d = extend_euclid(eulern, e);
	//key setting
	//e n public key
	//d n private key

	int crypto_val = encrypt(data, publickey);
	int message = decrypt(crypto_val, privatekey);

	fprintf(fp,"origin data : %d\n", data);
	fprintf(fp,"p = %d, q = %d, e = %d, n = %d\n", p, q, e, n);
	fprintf(fp,"d value : %d\n", privatekey.d);
	fprintf(fp,"crypto val : %d\n", crypto_val);
	fprintf(fp,"message : %d\n", message);


	return 0;
}