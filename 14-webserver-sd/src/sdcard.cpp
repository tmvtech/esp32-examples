#include "sdcard.h"
#include "SPI.h"
#include "SD.h"

gpio_num_t tSDCard::getCS() {return m_cs;}

bool tSDCard::begin()
{
    while (m_inuse) // wait until the sd card is free
        vTaskDelay(20 / portTICK_PERIOD_MS); // you can make this delay smaller
    
    m_inuse = true; // sets the in-use flag
    if (SD.begin(m_cs, SPI, 4000000)) // try to start the sd card
        return true; // success

    m_inuse = false; // reset in-use flag
    return false; // error
}

void tSDCard::end() 
{ 
    SD.end(); // end sd card use
    m_inuse = false; // reset in-use flag
}

bool tSDCard::available()
{
    bool hascard = true; // return value
    if (!begin() || SD.cardType() == CARD_NONE) // card does not exist?
        hascard = false;
    end(); // end sd card use
    return hascard;
}

uint16_t tSDCard::printFolderContents(String path, bool autoBegEnd)
{
    if (autoBegEnd && !begin()) // if should auto begin, but failed
    {
        Serial.println("SD: Card not available.");
        return 0;
    }

    File dir = SD.open(path); // open folder
    if (!dir) // folder does not exist
    {
        Serial.println("SD: No folder");
        if (autoBegEnd)
            end();
        return 0;
    }
    uint16_t fcount = 0; // file folder count
    while (File file = dir.openNextFile()) // for all files and folders
    {
        Serial.println(file.name()); // print file folder name
        fcount++; // increase file folder count
    }

    if (autoBegEnd) // should auto end?
        end(); // end sd card use
    return fcount; // return file folder count
}

uint16_t tSDCard::readAllText(char* out, uint16_t max, String path, bool autoBegEnd)
{
    if (autoBegEnd && !begin()) // if should auto begin, but failed
    {
        Serial.println("SD: Card not available.");
        return 0;
    }

    File f = SD.open(path); // open file
    if (!f || f.size() == 0) // file doesnt exist or is empty
    {
        Serial.println("SD: file empty or does not exist");
        if (autoBegEnd)
            end();
        return 0; 
    }
    uint16_t len = 0; // read length
    int c = f.read(); // read first char
    while (c != '\0' && c != EOF && len < (max - 1)) // until null or end of file
    {
        out[len] = c; // save char
        len++; // increment len
        c = f.read(); // read next char
    }
    out[len] = '\0'; // make sure the string is null terminated

    if (autoBegEnd) // if should auto end
        end();

    return len;
}

uint16_t tSDCard::readAllBytes(uint8_t* out, uint16_t max, String path, bool autoBegEnd)
{
    if (autoBegEnd && !begin()) // if should auto begin, but failed
    {
        Serial.println("SD: Card not available");
        return 0;
    }

    File f = SD.open(path);
    if (!f || f.size() == 0) // file doesnt exist or is empty
    {
        Serial.println("SD: file empty or does not exist");
        if (autoBegEnd)
            end();
        return 0; 
    }
    uint16_t len = 0;
    int c = f.read(); // read first byte
    while (c != EOF && len < max) // until end of file or buffer full
    {
        out[len] = c; // store char
        len++; // increase length
        c = f.read(); // read next
    }

    if (autoBegEnd) // if should auto end
        end();
    return len;
}

File tSDCard::openRead(String path) 
{ 
    if (!SD.exists(path)) // file does not exist?
        return File(nullptr); // return null handle
    return SD.open(path); // open file
}

size_t tSDCard::fileSize(String path, bool autoBegEnd)
{
    if (autoBegEnd && !begin()) // if should auto begin, but failed
    {
        Serial.println("SD: Card not available");
        return 0;
    }

    if (!SD.exists(path)) // file does not exist?
    {
        if (autoBegEnd)
            end();
        return 0;
    }
    File f = SD.open(path); // open file
    if (!f) // null handle?
    {
        if (autoBegEnd)
            end();
        return 0;
    }
    size_t out = f.size(); // get size
    f.close(); // close file
    if (autoBegEnd)
        end();
    return out; // return size
}