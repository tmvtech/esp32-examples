#ifndef _sdcard_h_
#define _sdcard_h_

#include <Arduino.h>
#include <SD.h>

/* @brief Class that manages the SD Card */
class tSDCard
{
    private:
    // cs pin
    gpio_num_t m_cs;
    // true if the card is being used
    volatile bool m_inuse;

    public:
    /* @brief Creates a new tSDCard. By default Pin 4 is used */
    tSDCard() : m_cs(GPIO_NUM_4), m_inuse(false) {}
    /* @brief Creates a new tSDCard
     * @param cs The CS pin to use */
    tSDCard(gpio_num_t cs) : m_cs(cs), m_inuse(false) {}

    /* @brief Get CS pin */
    gpio_num_t getCS();

    /* @brief Begins and ends the SD card, telling if it is working */
    bool available();
    /* @brief Begins the SD card, blocks until free to use
     * @warning Must call end() after using */
    bool begin();
    /* @brief Ends the use of the SD card */
    void end();

    /* @brief Prints the contents of a folder to the console */
    uint16_t printFolderContents(String path, bool autoBegEnd = true);
    /* @brief Read a file and store it's contents on the given buffer */
    uint16_t readAllText(char* out, uint16_t max, String path, bool autoBegEnd = true);
    /* @brief Read a file and store it's contents on the given buffer */
    uint16_t readAllBytes(uint8_t* out, uint16_t max, String path, bool autoBegEnd = true);

    /* @brief Open a file in read mode, must call close when done 
     * @warning Must call begin() before using */
    File openRead(String path);
    /* @brief Returns the size of a file */
    size_t fileSize(String path, bool autoBegEnd = true);
};

#endif
