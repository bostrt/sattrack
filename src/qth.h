#ifndef _QTH_H
#define _QTH_H

typedef struct QTH {
  char callsign[7] = "KN4CUV";
  double latitude = 35.751489; // In degrees
  double longitude = -78.775148; // In degrees
  double elevation = 152; // In meters
  // TODO: Store TLE
};

class QTH2 {
public:
  String  callsign = "KN4CUV";
  double latitude = 35.751489; // In degrees
  double longitude = -78.775148; // In degrees
  double elevation = 152; // In meters
  // TODO: Store TLE
};

#endif
