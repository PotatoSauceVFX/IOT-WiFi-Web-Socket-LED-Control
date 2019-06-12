// EEPROM Initialization
bool EEPROM_initialize(size_t size)
{
    if (!EEPROM.begin(size))
    {
        Serial << "Failed to initialise EEPROM" << endl;
        return false;
    }

    return true;
}

// EEPROM Write Data
template <class T>
int EEPROM_writeAnything(int ee, const T &value)
{
    const byte *p = (const byte *)(const void *)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
    {
        EEPROM.write(ee++, *p++);
    }
    EEPROM.commit();
    return i;
}

// EEPROM Read Data
template <class T>
int EEPROM_readAnything(int ee, T &value)
{
    byte *p = (byte *)(void *)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
    {
        *p++ = EEPROM.read(ee++);
    }
    return i;
}
