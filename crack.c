#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sha256.h"

int iPwd4Len;

int iNumOfHashesWanted = 0;
int iNumOfHashedFound;

int iNumOfHashesInFile = 0;
unsigned char **pHashesInFile;

int iNumOfPasswordsInFile = 0;
unsigned char **pPasswordInFile;

void ReadPasswords(const char *path);
void ReadHashes(const char *path);

void GuessAllPwd4();
void GuessPwd6AllNum();
void GuessPwd6AllLetters();  // AllLetters means all lowercase letters

int CompareHashes(unsigned char *pHashGuessed, unsigned char *pHashInFile);

void FreeHashes();
void FreePasswords();

#define MAX_LINE 1024		// Any line in the password file should not be larger than 1K

void main(int argc, char *argv[])
{
	iPwd4Len = pow(96, 4);

	switch (argc)
	{
		case 1:			// Run with no argument
		{
			ReadHashes(".\\pwd4sha256");
			GuessAllPwd4();

			ReadHashes(".\\pwd6sha256");
			iNumOfHashesWanted = 0;
			iNumOfHashedFound = 0;
			GuessPwd6AllNum();
			GuessPwd6AllLetters();

			break;
		}

		case 2:			//	Run with one integer argument
		{
			iNumOfHashesWanted = atoi(argv[1]);
			iNumOfHashedFound = 0;

			ReadHashes(".\\pwd4sha256");
			GuessAllPwd4();

			ReadHashes(".\\pwd6sha256");
			GuessPwd6AllNum();
			GuessPwd6AllLetters();

			break;
		}

		case 3:			// Run with two arguments
		{
			ReadPasswords(argv[1]);
			ReadHashes(argv[2]);

			SHA256_CTX ctx;

			for (int i = 0; i < iNumOfPasswordsInFile; i++)
			{
				for (int j = 0; j < iNumOfHashesInFile; j++)
				{
					unsigned char hash[32];
					sha256_init(&ctx);
					sha256_update(&ctx, pPasswordInFile[i], strlen(pPasswordInFile[i]));
					sha256_final(&ctx, hash);

					if (CompareHashes(hash, pHashesInFile[j]) == 0)
					{
						printf("%s %i\n", pPasswordInFile[i], j + 1);

						iNumOfHashedFound++;

						if (iNumOfHashesWanted != 0 && iNumOfHashedFound >= iNumOfHashesWanted)
						{
							return;
						}
					}
				}
			}

			break;
		}

		default:
		{
			break;
		}
	}

	FreeHashes();
	FreePasswords();
}

int CompareHashes(unsigned char *pHashGuessed, unsigned char *pHashInFile)
{
	for (int i = 0; i < 32; i++)
	{
		if (*(pHashGuessed + i) != *(pHashInFile + i))
			return -1;
	}

	return 0;
}

void ReadPasswords(const char *path)
{
	FILE *fp = fopen(path, "rb");

	if (NULL == fp)
	{
		printf("File not found.\n");
		exit(1);
	}

	FreePasswords();

	iNumOfPasswordsInFile = 0;
	while (!feof(fp))
	{
		if (fgetc(fp) == '\n')
			iNumOfPasswordsInFile++;
	}

	pPasswordInFile = (unsigned char **)malloc(sizeof(unsigned char *) * iNumOfPasswordsInFile);

	fseek(fp, 0, SEEK_SET);

	int index = 0;
	char buf[MAX_LINE];
	while (fgets(buf, MAX_LINE, fp) != NULL)
	{
		size_t len = strlen(buf);
		buf[len - 1] = '\0';  

		pPasswordInFile[index] = (unsigned char *)malloc(sizeof(unsigned char) * len);
		memcpy(pPasswordInFile[index], buf, len);

		index++;
	}

	fclose(fp);
}

void ReadHashes(const char *path)
{
	FILE *fp = fopen(path, "rb");

	if (NULL == fp)
	{
		printf("File not found.\n");
		exit(1);
	}

	FreeHashes();

	fseek(fp, 0, SEEK_END);
	iNumOfHashesInFile = ftell(fp) / 32;
	fseek(fp, 0, SEEK_SET);

	pHashesInFile = (unsigned char **)malloc(sizeof(unsigned char *) * iNumOfHashesInFile);

	for (int i = 0; i < iNumOfHashesInFile; i++)
	{
		pHashesInFile[i] = (unsigned char *)malloc(sizeof(unsigned char) * 32);
	}
	
	int i = 0;
	unsigned char c;
	while (i < iNumOfHashesInFile)
	{
		for (int j = 0; j < 32; j++)
		{
			c = (unsigned char)fgetc(fp);
			pHashesInFile[i][j] = c;
		}
		i++;
	}

	fclose(fp);
}

void FreeHashes()
{
	for (int i = 0; i < iNumOfHashesInFile; i++)
	{
		if (pHashesInFile[i] != NULL)
		{
			free(pHashesInFile[i]);
		}
	}

	if (pHashesInFile != NULL)
	{
		free(pHashesInFile);
	}

	iNumOfHashesInFile = 0;
}

void FreePasswords()
{
	for (int i = 0; i < iNumOfPasswordsInFile; i++)
	{
		if (pPasswordInFile[i] != NULL)
		{
			free(pPasswordInFile[i]);
		}
	}

	if (pPasswordInFile != NULL)
	{
		free(pPasswordInFile);
	}

	iNumOfPasswordsInFile = 0;
}

void GuessPwd6AllNum()
{
	if (iNumOfHashesWanted != 0 && iNumOfHashedFound >= iNumOfHashesWanted)
	{
		return;
	}

	SHA256_CTX ctx;

	for (unsigned char i = 0; i < 10; i++)
	{
		for (unsigned char j = 0; j < 10; j++)
		{
			for (unsigned char k = 0; k < 10; k++)
			{
				for (unsigned char l = 0; l < 10; l++)
				{
					for (unsigned char m = 0; m < 10; m++)
					{
						for (unsigned char n = 0; n < 10; n++)
						{
							unsigned char candidate[7] = { '\0' };
							candidate[0] = 48 + i;
							candidate[1] = 48 + j;
							candidate[2] = 48 + k;
							candidate[3] = 48 + l;
							candidate[4] = 48 + m;
							candidate[5] = 48 + n;

							unsigned char hash[32];
							sha256_init(&ctx);
							sha256_update(&ctx, candidate, 6);
							sha256_final(&ctx, hash);

							for (int x = 0; x < iNumOfHashesInFile; x++)
							{
								if (CompareHashes(hash, pHashesInFile[x]) == 0)
								{
									printf("%s %i\n", candidate, x + 11);

									iNumOfHashedFound++;

									if (iNumOfHashesWanted != 0 && iNumOfHashedFound >= iNumOfHashesWanted)
									{
										return;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void GuessPwd6AllLetters()
{
	if (iNumOfHashesWanted != 0 && iNumOfHashedFound >= iNumOfHashesWanted)
	{
		return;
	}

	SHA256_CTX ctx;

	for (unsigned char i = 0; i < 26; i++)
	{
		for (unsigned char j = 0; j < 26; j++)
		{
			for (unsigned char k = 0; k < 26; k++)
			{
				for (unsigned char l = 0; l < 26; l++)
				{
					for (unsigned char m = 0; m < 26; m++)
					{
						for (unsigned char n = 0; n < 26; n++)
						{
							unsigned char candidate[7] = { '\0' };
							candidate[0] = 65 + i;
							candidate[1] = 65 + j;
							candidate[2] = 65 + k;
							candidate[3] = 65 + l;
							candidate[4] = 65 + m;
							candidate[5] = 65 + n;

							unsigned char hash[32];
							sha256_init(&ctx);
							sha256_update(&ctx, candidate, 6);
							sha256_final(&ctx, hash);

							for (int x = 0; x < iNumOfHashesInFile; x++)
							{
								if (CompareHashes(hash, pHashesInFile[x]) == 0)
								{
									printf("%s %i\n", candidate, x + 11);

									iNumOfHashedFound++;

									if (iNumOfHashesWanted != 0 && iNumOfHashedFound >= iNumOfHashesWanted)
									{
										return;
									}
								}
							}

							candidate[0] = 97 + i;
							candidate[1] = 97 + j;
							candidate[2] = 97 + k;
							candidate[3] = 97 + l;
							candidate[4] = 97 + m;
							candidate[5] = 97 + n;

							sha256_init(&ctx);
							sha256_update(&ctx, candidate, 6);
							sha256_final(&ctx, hash);

							for (int x = 0; x < iNumOfHashesInFile; x++)
							{
								if (CompareHashes(hash, pHashesInFile[x]) == 0)
								{
									printf("%s %i\n", candidate, x + 11);

									iNumOfHashedFound++;

									if (iNumOfHashesWanted != 0 && iNumOfHashedFound >= iNumOfHashesWanted)
									{
										return;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void GuessAllPwd4()
{
	if (iNumOfHashesWanted != 0 && iNumOfHashedFound >= iNumOfHashesWanted)
	{
		return;
	}

	SHA256_CTX ctx;

	for (unsigned char i = 0; i < 96; i++)
	{
		for (unsigned char j = 0; j < 96; j++)
		{
			for (unsigned char k = 0; k < 96; k++)
			{
				for (unsigned char l = 0; l < 96; l++)
				{
					unsigned char candidate[5] = {'\0'};
					candidate[0] = 32 + i;
					candidate[1] = 32 + j;
					candidate[2] = 32 + k;
					candidate[3] = 32 + l;

					unsigned char hash[32];
					sha256_init(&ctx);
					sha256_update(&ctx, candidate, 4);
					sha256_final(&ctx, hash);

					for (int x = 0; x < iNumOfHashesInFile; x++)
					{
						if (CompareHashes(hash, pHashesInFile[x]) == 0)
						{
							printf("%s %i\n", candidate, x + 1);

							iNumOfHashedFound++;

							if (iNumOfHashesWanted != 0 && iNumOfHashedFound >= iNumOfHashesWanted)
							{
								return;
							}
						}
					}
				}
			}
		}
	}
}