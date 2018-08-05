#ifndef _TLE_H
#define _TLE_H

#define NAME_LIMIT 32
#define TLE_LINE_LENGTH 70

typedef struct Satellite {
  char name[NAME_LIMIT];
  char tle1[TLE_LINE_LENGTH];
  char tle2[TLE_LINE_LENGTH];
};

#endif
