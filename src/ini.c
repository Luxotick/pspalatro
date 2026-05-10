#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "global.h"

FILE *g_ini_file = NULL;

bool ini_open(const char *filename)
{
    g_ini_file = fopen(filename, "r");
    if (g_ini_file == NULL)
    {
        DEBUG_PRINTF("Error: Could not open INI file\n");
        return false;
    }

    return true;
}

void ini_close()
{
    if (g_ini_file != NULL)
    {
        fclose(g_ini_file);
    }
}

enum IniTokenType g_ini_last_token = INI_TOKEN_ERROR;
enum IniTokenType g_ini_next_token = INI_TOKEN_ERROR;

enum IniTokenType ini_read_token(char *buffer, int buffer_size)
{
    if (g_ini_file == NULL) return INI_TOKEN_ERROR;

    int i = 0;
    char c;

    do
    {
        c = fgetc(g_ini_file);
        if (c == EOF) return INI_TOKEN_EOF;
        if (c == '=')
        {
            if (g_ini_last_token == INI_TOKEN_KEY)
            {
                g_ini_next_token = INI_TOKEN_VALUE;
            }
            else
            {
                return INI_TOKEN_ERROR;
            }
        }
    } while (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '=');
    
    if (c == '[')
    {
        if (g_ini_last_token == INI_TOKEN_ERROR ||
            g_ini_last_token == INI_TOKEN_VALUE ||
            g_ini_last_token == INI_TOKEN_SECTION)
        {
            g_ini_next_token = INI_TOKEN_SECTION;
            c = fgetc(g_ini_file);
        }
        else
        {
            return INI_TOKEN_ERROR;
        }
    }

    while (i < buffer_size - 1)
    {
        if (i > 0) c = fgetc(g_ini_file);
        if (isalpha(c) || isdigit(c) || c == '_' || c == '-' || c == '.')
        {
            buffer[i++] = c;
        }
        else if (g_ini_next_token == INI_TOKEN_SECTION && c == ']')
        {
            buffer[i] = '\0';
            g_ini_last_token = INI_TOKEN_SECTION;
            g_ini_next_token = INI_TOKEN_ERROR;
            return INI_TOKEN_SECTION;
        }
        else if (c == '=')
        {
            if (g_ini_last_token == INI_TOKEN_ERROR ||
                g_ini_last_token == INI_TOKEN_VALUE ||
                g_ini_last_token == INI_TOKEN_SECTION)
            {
                buffer[i] = '\0';
                g_ini_last_token = INI_TOKEN_KEY;
                g_ini_next_token = INI_TOKEN_VALUE;
                return INI_TOKEN_KEY;                
            }
            else
            {
                return INI_TOKEN_ERROR;
            }
        }
        else if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
        {
            if (g_ini_next_token == INI_TOKEN_VALUE)
            {
                buffer[i] = '\0';
                g_ini_last_token = INI_TOKEN_VALUE;
                g_ini_next_token = INI_TOKEN_ERROR;
                return INI_TOKEN_VALUE;
            }
        }
        else
        {
            return INI_TOKEN_ERROR;
        }
    }
    buffer[i] = '\0';

    return INI_TOKEN_ERROR;
}

