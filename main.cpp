#include <iostream>
#include <cmath>
#include <string>

unsigned int key[8]{
    0x1FFFFFFF,
    0x2FFFFFFF,
    0x3FFFFFFF,
    0x4FFFFFFF,
    0x5FFFFFFF,
    0x6FFFFFFF,
    0x7FFFFFFF,
    0x8FFFFFFF};

unsigned int s_block[8][16]{
    {9, 6, 3, 2, 8, 11, 1, 7, 10, 4, 14, 15, 12, 0, 13, 5},
    {3, 7, 14, 9, 8, 10, 15, 0, 5, 2, 6, 12, 11, 4, 13, 1},
    {14, 4, 6, 2, 11, 3, 13, 8, 12, 15, 5, 10, 0, 7, 1, 9},
    {14, 7, 10, 12, 13, 1, 3, 9, 0, 2, 11, 4, 15, 8, 5, 6},
    {11, 5, 1, 9, 8, 13, 15, 0, 14, 4, 2, 3, 12, 7, 10, 6},
    {3, 10, 13, 12, 1, 2, 0, 11, 7, 5, 9, 4, 8, 15, 14, 6},
    {1, 13, 2, 9, 7, 10, 6, 0, 8, 12, 4, 5, 15, 3, 11, 14},
    {11, 10, 15, 5, 0, 12, 14, 8, 6, 2, 3, 9, 1, 7, 13, 4},
};

unsigned int addition(unsigned int rightBlock, unsigned int subKey)
{
    return (rightBlock + subKey) % 4294967296;
}

unsigned int subtitution(unsigned int rightBlock)
{
    unsigned int s_right;
    unsigned int newRightBlock = 0;

    for (int j = 0; j < 8; j++)
    {
        unsigned char index = rightBlock & 0x000F;
        rightBlock = rightBlock >> 4;
        s_right = s_block[j][index];
        newRightBlock = (newRightBlock << 4) | s_right;
    }

    return newRightBlock;
}

unsigned int rotation(unsigned int rightBlock)
{
    return (rightBlock << 11) | (rightBlock >> (32 - 11));
}

unsigned long int enCrypt(unsigned long int block)
{
    unsigned int rightBlock = block & 0x00000000FFFFFFFF;
    unsigned int leftBlock = (block >> 32) & 0x00000000FFFFFFFF;
    unsigned int temp;

    for (int i = 0; i < 32; i++)
    {
        // 4294967296 = 2^32;
        unsigned int temp = leftBlock;
        leftBlock = rightBlock;

        if (i < 24)
            addition(rightBlock, key[i % 8]);
        else
            addition(rightBlock, key[31 - i]);

        // S-блок
        rightBlock = subtitution(rightBlock);

        // циклический сдвиг
        rightBlock = rotation(rightBlock);

        // исключающее ИЛИ
        rightBlock = temp ^ rightBlock;
    }

    return (((unsigned long int)leftBlock << 32) & 0xFFFFFFFF00000000) | ((unsigned long int)rightBlock & 0x00000000FFFFFFFF);
};

unsigned long int deCrypt(unsigned long int block)
{
    unsigned int leftBlock = block & 0x00000000FFFFFFFF;
    unsigned int rightBlock = (block >> 32) & 0x00000000FFFFFFFF;
    unsigned int temp;

    for (int i = 0; i < 32; i++)
    {
        // 4294967296 = 2^32;
        unsigned int temp = leftBlock;
        leftBlock = rightBlock;

        if (i < 8)
        {
            addition(rightBlock, key[i]);
        }
        else
        {
            addition(rightBlock, key[(31 - i) % 8]);
        }

        // S-блок
        rightBlock = subtitution(rightBlock);

        // циклический сдвиг
        rightBlock = rotation(rightBlock);

        // исключающее ИЛИ
        rightBlock = temp ^ rightBlock;
    }

    return (((unsigned long int)leftBlock) & 0x00000000FFFFFFFF) | (((unsigned long int)rightBlock << 32) & 0xFFFFFFFF00000000);
};

int main(int, char **)
{
    // std::cout << "Введите текст: " << std::endl;
    // std::string text = "";
    // std::cin >> text;
    std::string text = "Lilo and Stitch";
    std::cout << text << std::endl << std::endl;

    if (text.length() % 8 != 0)
    {
        for (int i = 0; i < text.length() % 8; i++)
        {
            text.append("*");
        }
    }

    unsigned long int length = text.length() / 8;
    unsigned long int *blocks = new unsigned long int[length];

    for (int i = 0; i < length; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            blocks[i] = blocks[i] << 8;
            blocks[i] = (blocks[i] & 0xFFFFFFFFFFFFFF00) | (unsigned long int)text[i * 8 + j];
        }
    }

    unsigned long int encryptedBlock;
    unsigned long int decryptedBlock;
    std::string decryptedText = "";
    std::string encryptedText = "";

    for (int i = 0; i < length; i++)
    {
        encryptedBlock = enCrypt(blocks[i]);
        decryptedBlock = deCrypt(encryptedBlock);
        
        for (int j = 0; j < 8; j++)
        {
            encryptedText += (char)((encryptedBlock >> 56 - j * 8) & 0xFF);
            decryptedText += (char)((decryptedBlock >> 56 - j * 8) & 0xFF);
        }
    }
    
    std::cout << "encryptedText: " << encryptedText << std::endl;
    std::cout << "decryptedText: " << decryptedText << std::endl;
    
    delete blocks;
}
