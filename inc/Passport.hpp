#pragma once

#include <fstream>
#include <ostream>
#include <string>

/**==========================Типы данных==========================
 * ========Поле 62========|================Поле 63================
 * 1 Исходные данные      | 1  HRPT NOAA
 *                        | 11 GMS S-VISSR
 * ---------------------------------------------------------------
 * 2 Одноканальные данные | 1  NOAA AVHRR
 *                        | 11 GMS S-VISSR
 * ---------------------------------------------------------------
 * 3 Проекция             | 1  NOAA AVHRR
 *                        | 11 GMS S-VISSR
 * ---------------------------------------------------------------
 * 4 Данные телеметрии    | 1  NOAA/AVHRR
 *                        | 11 GMS S-VISSR (данные из DOC-сектора)
 * ---------------------------------------------------------------
*/

struct Date {
  Date () {};
  Date (short year, short month, short day) : year(year), month(month), day(day) {};
  short year, month, day;
};

struct Time {
  Time () {};
  Time (short hour, short minute, short second, short millisecond) : hours(hour), minutes(minute), seconds(second), milliseconds(millisecond) {};
  short hours, minutes, seconds, milliseconds;
};

namespace satellite {
namespace passport{

enum SAT_ID {//Идентификаторы ИСЗ
  NOAA_9  = 15427,
  NOAA_10 = 16969,
  NOAA_11 = 19531,
  NOAA_12 = 21263,
  NOAA_14 = 23455,
  NOAA_15 = 25338,
  NOAA_16 = 26536,
  NOAA_17 = 27453,
  NOAA_19 = 33591,
  GMS_5   = 23522,
  FY_1c   = 25730,
  FY_1d   = 27431
};

enum class DATA_TYPE : uint8_t {
  INITIAL_DATA = 1,
  ONE_CHANNEL = 2,
  PROECTION = 3,
  TELEMETRY_DATA = 4
};

#pragma pack(push, 1)
struct Default {
public:
  Default () {};
  Default ( const passport::Default& src );
  Default ( const std::string& fileName );
  inline uint8_t getFormatType () {
    return FFh1_FORMAT;
  };
  inline const std::string getName () const {
    return std::string(SAT2_NAME);
  };
  inline uint32_t getId () {
    return SAT_ID;
  };
  inline uint32_t getNodeNum () {
    return NODE_NUM;
  };
  const Date getDate () const;
  const Time getTime () const;
  const std::string getDataType () const;
// protected:
  uint8_t FFh1_FORMAT;  //0 -- Тип формата - FFh1
  char SAT2_NAME[14];   //1 -- Название ИСЗ2, заглавными буквами
  uint32_t SAT_ID;      //14 -- Идентификатор ИСЗ (см. enum SAT_ID)
  uint32_t NODE_NUM;    //18 -- Номер витка
  uint16_t DATE_YEAR;   //22 -- Дата начала приема (UTC) --- год
  uint16_t DATE_DAY;    //24 --                    --- день года (1-based)
  uint32_t DATE_TIME;   //26 --                    --- время начала приема(миллисекунд с начала дня)
  uint8_t RESERVE_1[8]; //30 -- Резерв
  uint8_t SERVICE_1[22];//38 -- Служебные
  uint8_t RESERVE_2[2]; //60 -- Резерв
  uint8_t DATA_TYPE[2]; //62 -- Тип данных (см. комментарий "Типы данных")
  friend std::ifstream& operator>> ( std::ifstream& file, passport::Default& pass ) {

    if (!file.is_open()) return file;

    file.read(reinterpret_cast<char *>(&(pass.FFh1_FORMAT)), 1);
    file.read(pass.SAT2_NAME, 13); pass.SAT2_NAME[13] = '\0';
    file.read(reinterpret_cast<char *>(&(pass.SAT_ID)), sizeof(SAT_ID));
    file.read(reinterpret_cast<char *>(&(pass.NODE_NUM)), sizeof(NODE_NUM));
    file.read(reinterpret_cast<char *>(&(pass.DATE_YEAR)), sizeof(DATE_YEAR));
    file.read(reinterpret_cast<char *>(&(pass.DATE_DAY)), sizeof(DATE_DAY));
    file.read(reinterpret_cast<char *>(&(pass.DATE_TIME)), sizeof(DATE_TIME));
    file.read(reinterpret_cast<char *>(pass.RESERVE_1), 8);
    file.read(reinterpret_cast<char *>(pass.SERVICE_1), 22);
    file.read(reinterpret_cast<char *>(pass.RESERVE_2), 2);
    file.read(reinterpret_cast<char *>(pass.DATA_TYPE), 2);

    return file;
  };
  friend std::ofstream& operator<< ( std::ofstream& file, const passport::Default& pass ) {
    if (!file.is_open()) return file;

    Date vDate = pass.getDate();
    Time vTime = pass.getTime();

    file << "Тип формата FFh1: "         << (short)pass.FFh1_FORMAT<< '\n'
         << "Название ИС32: "            << pass.SAT2_NAME         << '\n'
         << "Идентификатор ИСЗ: "        << pass.SAT_ID            << '\n'
         << "Номер витка: "              << pass.NODE_NUM          << '\n'
         << "Дата начала приема (UTC): " << vDate.day              << '/'
                                         << vDate.month            << '/'
                                         << vDate.year             << ' '
                                         << vTime.hours            << ':'
                                         << vTime.minutes          << ':'
                                         << vTime.seconds          << ':'
                                         << vTime.milliseconds     << '\n'
         << "Тип данных: "                << pass.getDataType();

    return file;
  };
  friend std::ostream& operator<< ( std::ostream& file, const passport::Default& pass ) {

    Date vDate = pass.getDate();
    Time vTime = pass.getTime();

    file << "Тип формата FFh1: "         << (short)pass.FFh1_FORMAT<< '\n'
         << "Название ИС32: "            << pass.SAT2_NAME         << '\n'
         << "Идентификатор ИСЗ: "        << pass.SAT_ID            << '\n'
         << "Номер витка: "              << pass.NODE_NUM          << '\n'
         << "Дата начала приема (UTC): " << vDate.day              << '/'
                                         << vDate.month            << '/'
                                         << vDate.year             << ' '
                                         << vTime.hours            << ':'
                                         << vTime.minutes          << ':'
                                         << vTime.seconds          << ':'
                                         << vTime.milliseconds     << '\n'
         << "Тип данных: "               << pass.getDataType();

    return file;
  }
  friend std::ofstream& operator< ( std::ofstream& file, const satellite::passport::Default& pass ) {
    if (!file.is_open()) return file;

    file.write((char*)(&(pass.FFh1_FORMAT)), 1);
    file.write((char*)(pass.SAT2_NAME), 13);
    file.write((char*)(&(pass.SAT_ID)), sizeof(SAT_ID));
    file.write((char*)(&(pass.NODE_NUM)), sizeof(NODE_NUM));
    file.write((char*)(&(pass.DATE_YEAR)), sizeof(DATE_YEAR));
    file.write((char*)(&(pass.DATE_DAY)), sizeof(DATE_DAY));
    file.write((char*)(&(pass.DATE_TIME)), sizeof(DATE_TIME));
    file.write((char*)(pass.RESERVE_1), 8);
    file.write((char*)(pass.SERVICE_1), 22);
    file.write((char*)(pass.RESERVE_2), 2);
    file.write((char*)(pass.DATA_TYPE), 2);

    return file;
  }
};
#pragma pack(pop)


#pragma pack(push, 1)
struct Proection : public passport::Default {
public:
  Proection () : Default() {};
  Proection ( const std::string& fileName );
  Proection ( const passport::Default& src ) : Default(src) {};
  Proection ( const passport::Proection& src );
  const std::string getStageData () const;
  inline unsigned short getCountPixelsInLine () const {
    return COUNT_PIXELS;
  }
  inline unsigned short getCountLines () const {
    return STRINGS_COUNT;
  }
  inline unsigned short& setCountPixelsInLine ( unsigned short src ) {
    return (COUNT_PIXELS = src);
  }
  inline unsigned short& setCountLines ( unsigned short src ) {
    return (STRINGS_COUNT = src);
  }
  inline unsigned short getChannelNum () const {
    return CHANNEL_NUM;
  }
  inline const std::string getProectionType () const {
    if (PROECTION_TYPE == 1)
      return "меркаторская";
    if (PROECTION_TYPE == 2)
      return "равнопромежуточная";
    return "ошибка";
  }
  inline float getLatitude () const {
    return LATITUDE;
  }
  inline float getLongitude () const {
    return LONGITUDE;
  }
  inline float getLatitudeSize () const {
    return SIZE_LATITUDE;
  }
  inline float getLongitudeSize () const {
    return SIZE_LONGITUDE;
  }
  inline float getLatitudeStep () const {
    return STEP_LATITUDE;
  }
  inline float getLongitudeStep () const {
    return STEP_LONGITUDE;
  }
  /*Вывод пасспорта проекции в файл в бинарном виде*/
  friend std::ofstream& operator< ( std::ofstream& file, const satellite::passport::Proection& pass ) {
    if (!file.is_open())
      return file;

      file.write((char *)(&(pass.FFh1_FORMAT)), 1);
      file.write((char*)(pass.SAT2_NAME), 13);
      file.write((char *)(&(pass.SAT_ID)), sizeof(SAT_ID));
      file.write((char *)(&(pass.NODE_NUM)), sizeof(NODE_NUM));
      file.write((char *)(&(pass.DATE_YEAR)), sizeof(DATE_YEAR));
      file.write((char *)(&(pass.DATE_DAY)), sizeof(DATE_DAY));
      file.write((char *)(&(pass.DATE_TIME)), sizeof(DATE_TIME));
      file.write((char *)(pass.RESERVE_1), 8);
      file.write((char *)(pass.SERVICE_1), 22);
      file.write((char *)(pass.RESERVE_2), 2);
      file.write((char *)(pass.DATA_TYPE), 2);

      file.write((char *)(&(pass.STAGE_DATA)), sizeof(STAGE_DATA));
      file.write((char *)(&(pass.CHANNEL_NUM)), sizeof(CHANNEL_NUM));
      file.write((char *)(&(pass.SIGNIFICANT_PIXELS_MAX_VALUE)), sizeof(SIGNIFICANT_PIXELS_MAX_VALUE));
      file.write((char *)(&(pass.PROECTION_TYPE)), sizeof(PROECTION_TYPE));
      file.write((char *)(&(pass.STRINGS_COUNT)), sizeof(STRINGS_COUNT));
      file.write((char *)(&(pass.COUNT_PIXELS)), sizeof(COUNT_PIXELS));
      file.write((char *)(&(pass.LATITUDE)), sizeof(LATITUDE));
      file.write((char *)(&(pass.LONGITUDE)), sizeof(LONGITUDE));
      file.write((char *)(&(pass.SIZE_LATITUDE)), sizeof(SIZE_LATITUDE));
      file.write((char *)(&(pass.SIZE_LONGITUDE)), sizeof(SIZE_LONGITUDE));
      file.write((char *)(&(pass.STEP_LATITUDE)), sizeof(STEP_LATITUDE));
      file.write((char *)(&(pass.STEP_LONGITUDE)), sizeof(STEP_LONGITUDE));
      file.write((char *)(&(pass.COEFFICIENT_A)), sizeof(COEFFICIENT_A));
      file.write((char *)(&(pass.COEFFICIENT_B)), sizeof(COEFFICIENT_B));
      file.write((char *)(pass.RESERVE_3), 10);
      file.write((char *)(&(pass.NODE_NUM)), sizeof(NODE_NUM));
      file.write((char *)(&(pass.ELEMENTS_NUM)), sizeof(ELEMENTS_NUM));
      file.write((char *)(&(pass.EFERMIDS_TYPE)), sizeof(EFERMIDS_TYPE));
      file.write((char *)(&(pass.NODE_YEAR)), sizeof(NODE_YEAR));
      file.write((char *)(&(pass.DAY_NUM)), sizeof(DAY_NUM));
      file.write((char *)(&(pass.AVERAGE_MOV)), sizeof(AVERAGE_MOV));
      file.write((char *)(&(pass.AIR_RESISTANCE)), sizeof(AIR_RESISTANCE));
      file.write((char *)(&(pass.OBLIQUITY)), sizeof(OBLIQUITY));
      file.write((char *)(&(pass.RIGHT_ASCENSION_ASCENDING_NODE)), sizeof(RIGHT_ASCENSION_ASCENDING_NODE));
      file.write((char *)(&(pass.ECCENTRICITY)), sizeof(ECCENTRICITY));
      file.write((char *)(&(pass.PERIGEE_ARGUMENT)), sizeof(PERIGEE_ARGUMENT));
      file.write((char *)(&(pass.MEAN_ANOMALY)), sizeof(MEAN_ANOMALY));
      file.write((char *)(pass.RESERVE_4), 54);
      file.write((char *)(&(pass.CORRECTION_VERSION_NUM)), sizeof(CORRECTION_VERSION_NUM));
      file.write((char *)(&(pass.MANDAREL_BOARD_CLOCK)), sizeof(MANDAREL_BOARD_CLOCK));
      file.write((char *)(&(pass.CORRECTION_TIME)), sizeof(CORRECTION_TIME));
      file.write((char *)(&(pass.ROLL)), sizeof(ROLL));
      file.write((char *)(&(pass.PITCH)), sizeof(PITCH));
      file.write((char *)(&(pass.YAW)), sizeof(YAW));
      file.write((char *)(pass.RESERVE_5), 226);

    return file;
  }
// private:
  uint32_t STAGE_DATA;                  // 64 -- Стадия обработки данных
  uint16_t CHANNEL_NUM;                 // 68 -- Номер канала (возможные значения: 1, 2, 3, 4, 5)
  int16_t SIGNIFICANT_PIXELS_MAX_VALUE; // 70 -- Максимальное значение среди значимых пикселов изображения
  uint16_t PROECTION_TYPE;              // 72 -- Тип проекции: 1 - меркаторская, 2 - равнопромежуточная
  uint16_t STRINGS_COUNT;               // 74 -- Количество строк
  uint16_t COUNT_PIXELS;                // 76 -- Количество пикселов в строке
  float LATITUDE;                       // 78 -- Широта (градусы)
  float LONGITUDE;                      // 82 -- Долгота (градусы)
  float SIZE_LATITUDE;                  // 86 -- Размер по широте (градусы)
  float SIZE_LONGITUDE;                 // 90 -- Размер по долготе (градусы)
  float STEP_LATITUDE;                  // 94 -- Шаг по широте (секунды)
  float STEP_LONGITUDE;                 // 98 -- Шаг по долготе (секунды)
  double COEFFICIENT_A;                 //102 -- Коэффициент A
  double COEFFICIENT_B;                 //110 -- Коэффициент B
  uint8_t RESERVE_3[10];                //118 -- Резерв
  uint32_t NODE_NUM;                    //128 -- Номер опорного витка (полный)
  uint16_t ELEMENTS_NUM;                //132 -- Номер набора элементов
  uint16_t EFERMIDS_TYPE;               //134 -- Тип эфемерид
  uint16_t NODE_YEAR;                   //136 -- Год (полный)
  double DAY_NUM;                       //138 -- День от начала года (1-based)
  double AVERAGE_MOV;                   //146 -- Среднее движение (рад/мин)
  double AIR_RESISTANCE;                //154 -- Сопротивление атмосферы (BSTAR Drag Term)
  double OBLIQUITY;                     //162 -- Наклонение орбиты (рад)
  double RIGHT_ASCENSION_ASCENDING_NODE;//170 -- Прямое восхождение восходящего узла (рад)
  double ECCENTRICITY;                  //178 -- Эксцентриситет
  double PERIGEE_ARGUMENT;              //186 -- Аргумент перигея (рад)
  double MEAN_ANOMALY;                  //194 -- Средняя аномалия (рад)
  uint8_t RESERVE_4[54];                //202 -- Резерв
  uint16_t CORRECTION_VERSION_NUM;      //256 -- Номер версии коррекции, 0 - отсутствие коррекции
  int16_t MANDAREL_BOARD_CLOCK;         //258 -- Поправка бортовых часов по TBUS, мсек
  int16_t CORRECTION_TIME;              //260 -- Поправка времени, мсек
  double ROLL;                          //262 -- крен (roll)
  double PITCH;                         //270 -- тангаж (pitch)
  double YAW;                           //278 -- рысканье (yaw)
  uint8_t RESERVE_5[226];               //286 -- Резерв
  friend std::ifstream& operator>> ( std::ifstream& file, passport::Proection& pass ) {

    if (!file.is_open()) return file;

    file.read(reinterpret_cast<char *>(&(pass.FFh1_FORMAT)), 1);
    file.read(pass.SAT2_NAME, 13); pass.SAT2_NAME[13] = '\0';
    file.read(reinterpret_cast<char *>(&(pass.SAT_ID)), sizeof(SAT_ID));
    file.read(reinterpret_cast<char *>(&(pass.NODE_NUM)), sizeof(NODE_NUM));
    file.read(reinterpret_cast<char *>(&(pass.DATE_YEAR)), sizeof(DATE_YEAR));
    file.read(reinterpret_cast<char *>(&(pass.DATE_DAY)), sizeof(DATE_DAY));
    file.read(reinterpret_cast<char *>(&(pass.DATE_TIME)), sizeof(DATE_TIME));
    file.read(reinterpret_cast<char *>(pass.RESERVE_1), 8);
    file.read(reinterpret_cast<char *>(pass.SERVICE_1), 22);
    file.read(reinterpret_cast<char *>(pass.RESERVE_2), 2);
    file.read(reinterpret_cast<char *>(pass.DATA_TYPE), 2);

    file.read(reinterpret_cast<char *>(&(pass.STAGE_DATA)), sizeof(STAGE_DATA));
    file.read(reinterpret_cast<char *>(&(pass.CHANNEL_NUM)), sizeof(CHANNEL_NUM));
    file.read(reinterpret_cast<char *>(&(pass.SIGNIFICANT_PIXELS_MAX_VALUE)), sizeof(SIGNIFICANT_PIXELS_MAX_VALUE));
    file.read(reinterpret_cast<char *>(&(pass.PROECTION_TYPE)), sizeof(PROECTION_TYPE));
    file.read(reinterpret_cast<char *>(&(pass.STRINGS_COUNT)), sizeof(STRINGS_COUNT));
    file.read(reinterpret_cast<char *>(&(pass.COUNT_PIXELS)), sizeof(COUNT_PIXELS));
    file.read(reinterpret_cast<char *>(&(pass.LATITUDE)), sizeof(LATITUDE));
    file.read(reinterpret_cast<char *>(&(pass.LONGITUDE)), sizeof(LONGITUDE));
    file.read(reinterpret_cast<char *>(&(pass.SIZE_LATITUDE)), sizeof(SIZE_LATITUDE));
    file.read(reinterpret_cast<char *>(&(pass.SIZE_LONGITUDE)), sizeof(SIZE_LONGITUDE));
    file.read(reinterpret_cast<char *>(&(pass.STEP_LATITUDE)), sizeof(STEP_LATITUDE));
    file.read(reinterpret_cast<char *>(&(pass.STEP_LONGITUDE)), sizeof(STEP_LONGITUDE));
    file.read(reinterpret_cast<char *>(&(pass.COEFFICIENT_A)), sizeof(COEFFICIENT_A));
    file.read(reinterpret_cast<char *>(&(pass.COEFFICIENT_B)), sizeof(COEFFICIENT_B));
    file.read(reinterpret_cast<char *>(pass.RESERVE_3), 10);
    file.read(reinterpret_cast<char *>(&(pass.NODE_NUM)), sizeof(NODE_NUM));
    file.read(reinterpret_cast<char *>(&(pass.ELEMENTS_NUM)), sizeof(ELEMENTS_NUM));
    file.read(reinterpret_cast<char *>(&(pass.EFERMIDS_TYPE)), sizeof(EFERMIDS_TYPE));
    file.read(reinterpret_cast<char *>(&(pass.NODE_YEAR)), sizeof(NODE_YEAR));
    file.read(reinterpret_cast<char *>(&(pass.DAY_NUM)), sizeof(DAY_NUM));
    file.read(reinterpret_cast<char *>(&(pass.AVERAGE_MOV)), sizeof(AVERAGE_MOV));
    file.read(reinterpret_cast<char *>(&(pass.AIR_RESISTANCE)), sizeof(AIR_RESISTANCE));
    file.read(reinterpret_cast<char *>(&(pass.OBLIQUITY)), sizeof(OBLIQUITY));
    file.read(reinterpret_cast<char *>(&(pass.RIGHT_ASCENSION_ASCENDING_NODE)), sizeof(RIGHT_ASCENSION_ASCENDING_NODE));
    file.read(reinterpret_cast<char *>(&(pass.ECCENTRICITY)), sizeof(ECCENTRICITY));
    file.read(reinterpret_cast<char *>(&(pass.PERIGEE_ARGUMENT)), sizeof(PERIGEE_ARGUMENT));
    file.read(reinterpret_cast<char *>(&(pass.MEAN_ANOMALY)), sizeof(MEAN_ANOMALY));
    file.read(reinterpret_cast<char *>(pass.RESERVE_4), 54);
    file.read(reinterpret_cast<char *>(&(pass.CORRECTION_VERSION_NUM)), sizeof(CORRECTION_VERSION_NUM));
    file.read(reinterpret_cast<char *>(&(pass.MANDAREL_BOARD_CLOCK)), sizeof(MANDAREL_BOARD_CLOCK));
    file.read(reinterpret_cast<char *>(&(pass.CORRECTION_TIME)), sizeof(CORRECTION_TIME));
    file.read(reinterpret_cast<char *>(&(pass.ROLL)), sizeof(ROLL));
    file.read(reinterpret_cast<char *>(&(pass.PITCH)), sizeof(PITCH));
    file.read(reinterpret_cast<char *>(&(pass.YAW)), sizeof(YAW));
    file.read(reinterpret_cast<char *>(pass.RESERVE_5), 226);

    return file;
  };
};
#pragma pack(pop)

};
};
