#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#define POW(X) (X * X)
#define MOD11(X) ((X > 0) ? (X % 11) : ((X + 1100) % 11))

typedef struct point {
	int x;
	int y;
}point;

void print_point(point a, FILE *fp) {
	fprintf(fp,"{%d, %d}\n", a.x, a.y);
}

int inverse_modenumber(int input) {
	int i ;
	for (i = 1; i < 11; i++) {
		if (MOD11(input * i) == 1)
			return i;
	}

	return 0;
	//error
}


point doubling(point a1, int e11) {
	int lamdaval = MOD11(MOD11(3 * POW(a1.x) + e11) * MOD11(inverse_modenumber(2 * a1.y)));
	point result = {0,0};

	result.x = MOD11((POW(lamdaval) - 2 * a1.x ));
	result.y = MOD11(((a1.x - result.x) * lamdaval - a1.y));

	return result;
}

point addtion(point a1, point a2) {
	int lamdaval = MOD11(MOD11(a2.y - a1.y) * MOD11(inverse_modenumber(a2.x - a1.x)));
	point result;
	result.x = MOD11((POW(lamdaval) - a1.x - a2.x));
	result.y = MOD11(((a1.x - result.x) * lamdaval - a1.y));
	return result;
}

point add_point(point a1, point a2, int e11, FILE* fp) {
	point result;
	if (a1.x == a2.x && a1.y == a2.y)
		result = doubling(a1, e11);
	else
		result = addtion(a1, a2);
	fprintf(fp,"add pointing : ");
	print_point(result, fp);

	return result;
}

point calc_npoint(int nb, point G, int e11, FILE* fp){
	int i = 0;
	point return_point;
	if (nb <= 1)
		return G;
	return_point = add_point(G, G, e11, fp);
	if (nb == 2)
		return return_point;
	for (i; i < nb - 2; i++) {
		return_point = add_point(G, return_point, e11, fp);
	}
	return return_point;
}

void encryption(point message, point G, point pb, int k, int e11, point *result_arr, FILE* fp){
	point y1 = calc_npoint(k, G, e11, fp);
	point kb = calc_npoint(k, pb, e11, fp);
	point y2 = add_point(message, kb, e11, fp);
	result_arr[0] = y1;
	result_arr[1] = y2;
	
}

point decryption(point *crypto, int nb, int e11, FILE* fp){
	point result;
	point y1_mid_result =calc_npoint(nb, crypto[0], e11, fp);
	y1_mid_result.y = -y1_mid_result.y;
	result = add_point(crypto[1], y1_mid_result, e11, fp);
	//crypto[1] - nb * crypto[0];
	// minus sign equals  switch y value  and plus sign

	return result;
}

//e11 mean GF11 >> moduler 11 field
int main() {
	int nb = 11; //private key
	int k = 9; // random seed
	int e11 = 1; // graph value, 6 discard, a = 1;
	point G = { 2, 7 };

	point pb; // public key
	point cm; // crypto message
	point message = {10, 9}; // plain message
	point crypto[2];
	point decrypt_message;
	FILE* fp = fopen("data1.dat", "w");
	
	fprintf(fp,"plain text message\n");
	print_point(message, fp);
	fprintf(fp,"nb = %d, k = %d, e11 = %d G = ", nb, k, e11);
	print_point(G, fp);

	fprintf(fp,"calc public key\n");
	pb = calc_npoint(nb, G, e11, fp);
	fprintf(fp,"result\n");
	print_point(pb, fp);

	fprintf(fp,"encrypte message\n");
	encryption(message, G, pb, k, e11, crypto, fp);
	fprintf(fp,"result\n");
	print_point(crypto[0], fp);
	print_point(crypto[1], fp);

	fprintf(fp,"decryption message\n");
	decrypt_message = decryption(crypto,nb,e11, fp);
	fprintf(fp,"result\n");
	print_point(decrypt_message, fp);


	return 1;
}
