#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

#define SD_CS_PIN 2

double sizeInKBs(uint64_t bytes) { return (double)bytes / (1024.0); }
double sizeInMBs(uint64_t bytes) { return (double)bytes / (1024.0 * 1024.0); }
double sizeInGBs(uint64_t bytes) { return (double)bytes / (1024.0 * 1024.0 * 1024.0); }

int sd_Rename(fs::FS &fs, const char* from, const char* to) { return fs.rename(from, to); }
int sd_DeleteFile(fs::FS &fs, const char* path) { return fs.remove(path); }
int sd_CreateFolder(fs::FS &fs, const char* path) { return fs.mkdir(path); }
int sd_DeleteFolder(fs::FS &fs, const char* path) { return fs.rmdir(path); }

void sd_ListDir(fs::FS &fs, const char* dirname)
{
    Serial.printf("Listing directory: %s\n", dirname);
    
    File root = fs.open(dirname);
    if (!root)
    {
        Serial.println("Can't open directory");
        return;
    }
    if (!root.isDirectory())
    {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            Serial.print(" D: ");
            Serial.println(file.name());
        }
        else 
        {
            Serial.print(" F: ");
            Serial.print(file.name());
            Serial.printf("  SIZE: %.2lfKBs\n", sizeInKBs(file.size()));
        }
        file = root.openNextFile();
    }
}

int sd_WriteFile(fs::FS &fs, const char* path, const char* text, bool append = false)
{
    File file = append ? fs.open(path, FILE_APPEND) : fs.open(path, FILE_WRITE);
    if (!file)
        return 0;
    if (!file.print(text))
        return 0;
    file.close();
    return 1;
}

int sd_ReadFile(fs::FS &fs, const char* path, char* out, int maxsize)
{
    File file = fs.open(path, "r");
    if (!file)
        return 0;
    int size = 0;
    while (file.available() && size < maxsize - 1)
    {
        out[size] = (char)file.read();
        size++;
    }
    out[size] = '\0';
    file.close();
    return size;
}

void sd_TestIO(fs::FS &fs, const char* path, int bufsize = 512, int writeops = 2048)
{
    Serial.println("Testing IO speeds...");
    uint32_t start = millis();
    File file = fs.open(path);
    if (!file)
    {
        Serial.println("sd_TestIO: Can't read test file");
        return;
    }
    uint8_t buf[bufsize] = { 'a' };
    size_t len = file.size(), flen = len;
    while (len)
    {
        size_t toRead = (len > bufsize) ? bufsize : len;
        file.read(buf, toRead);
        len -= toRead;
    }
    file.close();
    uint32_t end = millis() - start;
    Serial.printf("Read %u bytes in %ums\n", (uint32_t)flen, (uint32_t)end);

    start = millis();
    file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("sd_TestIO: Can't write to test file");
        return;
    }
    for (int i = 0; i < writeops; i++)
        file.write(buf, bufsize);
    file.close();
    end = millis() - start;
    Serial.printf("Wrote %u bytes in %ums\n", (uint32_t)(writeops * bufsize), (uint32_t)end);
}

void setup() 
{
    Serial.begin(115200);
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    Serial.println("Testing SD Card, do not eject!");
    
    // Restart Esp if can't init sd card
    if (!SD.begin(SD_CS_PIN))
    {
        // wait 2 seconds
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        // restart
        ESP.restart();
        return;
    }

    // Print used space on SD
    Serial.printf("Used space: %.2lfMB/%.2lfGB\n", sizeInMBs(SD.usedBytes()), sizeInGBs(SD.totalBytes()));

    // + Dir
    Serial.println("Testing dir create:");
    if (!sd_CreateFolder(SD, "/test"))
        Serial.println("sd_CreateFolder: Error");
    else sd_ListDir(SD, "/");
    
    // > Dir
    Serial.println("Testing dir rename:");
    if (!sd_Rename(SD, "/test", "/testrename"))
        Serial.println("sd_Rename: Error");
    else sd_ListDir(SD, "/");
    
    // - Dir
    Serial.println("Testing dir delete:");
    if (!sd_DeleteFolder(SD, "/testrename"))
        Serial.println("sd_DeleteFolder: Error");
    else sd_ListDir(SD, "/");


    // + File
    Serial.println("Testing file write");
    if (!sd_WriteFile(SD, "/test.txt", "hello", false))
        Serial.println("sd_WriteFile: Can't overwrite file");
    Serial.println("Testing file append");
    if (!sd_WriteFile(SD, "/test.txt", "\nappended", true))
        Serial.println("sd_WriteFile: Can't append to file");
    sd_ListDir(SD, "/");

    // r File
    Serial.println("Testing file read");
    char buf[128];
    if (!sd_ReadFile(SD, "/test.txt", buf, 128))
        Serial.println("sd_ReadFile: Can't read file");
    else Serial.printf("Read: '%s'\n", buf);

    // w File
    Serial.println("Testing file overwrite");
    if (!sd_WriteFile(SD, "/test.txt", "goodbye", false))
        Serial.println("sd_WriteFile: Can't overwrite file");
    else if (sd_ReadFile(SD, "/test.txt", buf, 128))
        Serial.printf("Read after overwrite: '%s'\n", buf);
    
    // > File
    Serial.println("Testing file rename:");
    if (!sd_Rename(SD, "/test.txt", "/testrename.txt"))
        Serial.println("sd_Rename: Error");
    else sd_ListDir(SD, "/");

    // - File
    Serial.println("Testing file delete:");
    if (!sd_DeleteFile(SD, "/testrename.txt"))
        Serial.println("sd_DeleteFile: Error");
    else sd_ListDir(SD, "/");

    sd_TestIO(SD, "/testIO.txt");

    SD.end();

    Serial.println("Testing complete, SD Card can now be ejected!");
}

void loop() {}