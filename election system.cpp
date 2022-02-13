#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <cctype> 
#include <cstdlib>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include <stdlib.h> 
#include <math.h>
#include <sstream>
#include <random>

#define PARTY_NUM 25

using namespace std;

// global variables
int total_voters = 0;
int total_null = 0;

//T_Province
char provinces[][3] = { "SJ","AL","CA","HE","GU","PU","LM","FO","SC" };

//T_Party
typedef struct T_Party { //The structure for the party is defined
	int votes;
	int male_votes;
	int female_votes;
	int age_votes[5];
	int province_votes[9];
}party;

// global political party list of 25 parties
party parties[PARTY_NUM];

//T_Voter
typedef struct T_Voter { //The structure for the voters is defined
	char id[10];
	char codelec[7];
	char sex;
	char date[9];
	char numjun[6];
	char name[31];
	char papellido[27];
	char sapellido[27];
	int age;
	int province_idx;
	bool fulfilled_with_the_vote;
	int party_voted_for;
	T_Voter* PtrNext;
}*PtrT_Voter;

//Function to add a voter, receives as a parameter the list of voters and the TEXT LINE of the electoral register
//Extract each of the attributes of the line and store them in a node of the voters list
void addvoter(PtrT_Voter& ListV, char added[118])
{
	PtrT_Voter Aux = new (T_Voter);
	Aux->PtrNext = ListV;
	ListV = Aux;
	int n;

	for (int ce = 0; ce < 9; ce++) {
		ListV->id[ce] = added[ce];
	}
	ListV->id[9] = '\0';

	for (int co = 10; co < 16; co++) {
		ListV->codelec[co - 10] = added[co];
	}
	ListV->codelec[6] = '\0';

	n = rand() % 2 + 1;
	ListV->sex = n == 1 ? '1' : '2';

	n = rand() % 70 + 18;
	ListV->age = n;

	n = rand() % 9;
	ListV->province_idx = n;

	for (int fe = 19; fe < 27; fe++) {
		ListV->date[fe - 19] = added[fe];
	}
	ListV->date[8] = '\0';

	for (int nu = 28; nu < 33; nu++) {
		ListV->numjun[nu - 28] = added[nu];
	}
	ListV->numjun[5] = '\0';

	for (int nom = 34; nom < 64; nom++) {
		ListV->name[nom - 34] = added[nom];
	}
	ListV->name[30] = '\0';

	for (int pa = 65; pa < 91; pa++) {
		ListV->papellido[pa - 65] = added[pa];
	}
	ListV->papellido[26] = '\0';

	for (int sa = 92; sa < 118; sa++) {
		ListV->sapellido[sa - 92] = added[sa];
	}
	ListV->sapellido[26] = '\0';
}

//Function to load the voters, it takes care of taking each of the lines
//directly from the text file and send it to the function in charge of extracting each one
// of the data and insert it into a list node.
void LoadVoters(PtrT_Voter& ListV)
{
	int cont = 1;
	time_t inicio, fin;
	inicio = time(NULL);
	FILE* archivo;
	fopen_s(&archivo, "PADRON_COMPLETO.txt", "r");
	if (NULL == archivo) {
		printf("Cant open the file");
	}
	else
	{
		char added[119];
		while (fgets(added, 119, archivo) != NULL)
		{
			if (cont % 2 != 0) {
				addvoter(ListV, added);
			}
			cont++;
		}
		fclose(archivo);
	}

	fin = time(NULL);
	cout << "----------------------------Electoral Resigter Loaded--------------------------------" << endl;
	printf("\n El cargado del padron ha tardado : %f segundos.\n", difftime(fin, inicio));
	cout << "----------------------------Electoral Resigter Loaded--------------------------------" << endl;
	system("pause");
}

//Function to release the voters in heap structures of the linked list until the list is NULL

void RealeaseVoters(PtrT_Voter& ListV)
{
	PtrT_Voter Aux = ListV;
	time_t inicio, fin;

	inicio = time(NULL);

	while (Aux != NULL) {
		ListV = ListV->PtrNext;
		delete(Aux);
		//Globalfree(Aux);
		Aux = ListV;
	}

	fin = time(NULL);

	cout << "----------------------------Padron liberado--------------------------------" << endl;
	printf("\n La liberacion del padron ha tardado : %f segundos.\n", difftime(fin, inicio));
	cout << "----------------------------Padron liberado--------------------------------" << endl;
	system("pause");
}


//Function to initialize the list of voters
void InitializeVoters(PtrT_Voter& ListV)
{
	ListV = NULL;
}

PtrT_Voter SearchVoter(PtrT_Voter& List, char cual[9])
{
	bool found = false;
	PtrT_Voter Aux;
	Aux = List;

	while ((!found == true) && (Aux != NULL))
	{
		if (strncmp(Aux->id, cual, 9) == 0)
			found = true;
		else
			Aux = Aux->PtrNext;
	}
	return Aux;
}

//Function to initialize the list of parties
void InitializeParties()
{
	for (int i = 0; i < PARTY_NUM; i++) {
		parties[i].votes = 0;
		parties[i].male_votes = 0;
		parties[i].female_votes = 0;
		for (int j = 0; j < 5;j++) {
			parties[i].age_votes[j] = 0;
		}
		for (int j = 0; j < 9;j++) {
			parties[i].province_votes[j] = 0;
		}
	}
}

// check age and update party votes based on that
void checkAge(int idx, PtrT_Voter Aux) {
	int age = Aux->age;
	if (age >= 18 && age <= 27)
		parties[idx].age_votes[0] += 1;
	else if (age >= 28 && age <= 37)
		parties[idx].age_votes[1] += 1;
	else if (age >= 38 && age <= 47)
		parties[idx].age_votes[2] += 1;
	else if (age >= 48 && age <= 57)
		parties[idx].age_votes[3] += 1;
	else if (age >= 58)
		parties[idx].age_votes[4] += 1;
}

// A function that iterates through the list of voters and cast a vote
// based on rand() function
void castVote(PtrT_Voter& List) {
	PtrT_Voter Aux;
	Aux = List;
	srand(time(NULL));
	while (Aux != NULL) {
		int n = rand() % 2;
		if (n == 0) {
			Aux->fulfilled_with_the_vote = false;
			Aux->party_voted_for = 0;
			total_null++;
		}
		else {
			Aux->fulfilled_with_the_vote = true;
			int party_rand = rand() % 25 + 1;
			Aux->party_voted_for = party_rand;
			parties[party_rand - 1].votes += 1;
			if (Aux->sex == '1') {
				parties[party_rand - 1].male_votes += 1;
			}
			else if (Aux->sex == '2') {
				parties[party_rand - 1].female_votes += 1;
			}
			checkAge(party_rand - 1, Aux);
			parties[party_rand - 1].province_votes[Aux->province_idx]++;
			total_voters++;
		}
		Aux = Aux->PtrNext;
	}
}

void reportType1() {
	int maxVotes = parties[0].votes, maxIdx = 0;
	float sum = parties[0].votes;
	for (int i = 1; i < PARTY_NUM; i++) {
		if (parties[i].votes > maxVotes) {
			maxVotes = parties[i].votes;
			maxIdx = i;
		}
		sum += parties[i].votes;
	}
	cout << "\n\n----------------------------Report Type 1--------------------------------" << endl;
	printf("Winning Party is: party%d\n", maxIdx + 1);
	printf("Total votes gained are: %d\n", maxVotes);
	printf("Votes percentage is: %.2f%\n", (maxVotes / sum) * 100);
	cout << "----------------------------Report Type 1--------------------------------\n\n" << endl;
	system("pause");
}

void reportType2() {
	float sum = parties[0].votes;
	for (int i = 1; i < PARTY_NUM; i++) {
		sum += parties[i].votes;
	}
	cout << "\n\n----------------------------Report Type 2--------------------------------" << endl;
	for (int i = 0; i < PARTY_NUM; i++) {
		printf("For party%d\n",i + 1);
		printf("Total votes gained are: %d\n", parties[i].votes);
		printf("Votes percentage is: %.2f%\n\n", (parties[i].votes / sum) * 100);
	}
	cout << "----------------------------Report Type 2--------------------------------\n\n" << endl;
	system("pause");
}

void reportType3() {
	cout << "\n\n----------------------------Report Type 3--------------------------------" << endl;
	for (int i = 0; i < PARTY_NUM; i++) {
		printf("For party%d\n", i + 1);
		printf("---Filter By Province---\n");
		for (int j = 0; j < 9; j++) {
			printf("For %s\nTotal Votes: %d\n", provinces[j], parties[i].province_votes[j]);
			printf("Votes Percentage: %.2f\n", (float(parties[i].province_votes[j]) / total_voters) * 100);
		}
		printf("---Filter By Province---\n\n");
		printf("---Filter By Gender---\n");
		printf("Votes Male: %d\n", parties[i].male_votes);
		printf("Percent male votes: %.2f\n", (float(parties[i].male_votes) / total_voters) * 100);
		printf("Votes Female: %d\n", parties[i].female_votes);
		printf("Percent female votes: %.2f\n", (float(parties[i].female_votes) / total_voters) * 100);
		printf("---Filter By Gender---\n\n");
		printf("---Filter By Age---\n");
		printf("Votes Young: %d\n", parties[i].age_votes[0]);
		printf("Percent young votes: %.2f\n", (float(parties[i].age_votes[0]) / total_voters) * 100);
		printf("Votes Young Adult: %d\n", parties[i].age_votes[1]);
		printf("Percent young adult votes: %.2f\n", (float(parties[i].age_votes[1]) / total_voters) * 100);
		printf("Votes Adult: %d\n", parties[i].age_votes[2]);
		printf("Percent adult votes: %.2f\n", (float(parties[i].age_votes[2]) / total_voters) * 100);
		printf("Votes Mature Adult: %d\n", parties[i].age_votes[3]);
		printf("Percent mature adult votes: %.2f\n", (float(parties[i].age_votes[3]) / total_voters) * 100);
		printf("Votes Older Adult: %d\n", parties[i].age_votes[4]);
		printf("Percent older adult votes: %.2f\n", (float(parties[i].age_votes[4]) / total_voters) * 100);
		printf("---Filter By Age---\n\n");
	}
	cout << "----------------------------Report Type 3--------------------------------\n\n" << endl;
	system("pause");
}

// function to generate report
void reports(PtrT_Voter& List, int ReportType) {
	printf("Total null votes are %d\n\n", total_null);
	switch (ReportType) {
	case 1:
		reportType1();
		break;
	case 2:
		reportType2();
		break;
	case 3:
		reportType3();
		break;
	default:
		printf("Invalid report type\n");
	}
}

void main()
{
	bool accomplished = false;
	bool cycle = true;
	int option = 0;
	PtrT_Voter ListV;
	InitializeVoters(ListV);
	InitializeParties();
	cout << "Please wait while the Electoral Register loads" << endl;
	LoadVoters(ListV);
	char searching[] = "100996791";
	PtrT_Voter Found = NULL;
	Found = SearchVoter(ListV, searching);
	if (Found != NULL)
	{
		cout << endl << "Founded the voter!! " << endl << endl;
		cout << "ID: " << Found->id << endl;
		cout << "Name: " << Found->name << endl;
		cout << "First Surname: " << Found->papellido << endl;
		cout << "Second Surname: " << Found->sapellido << endl;
		cout << "Sex: " << Found->sex << endl;
		cout << "Date: " << Found->date << endl;
		cout << "Electoral Code:  " << Found->codelec << endl;
		cout << "Numero de Junta: " << Found->numjun << endl;
	}
	else
		cout << " Voter not found !!! " << endl;
	// casting vote
	castVote(ListV);
	// generating report 1 2 3
	reports(ListV, 1);
	reports(ListV, 2);
	reports(ListV, 3);
	RealeaseVoters(ListV);
	cout << "Finalized" << endl;
	system("pause");
}