#include "gps.h"

static constexpr char* set_mtk_baud_115200 = "$PMTK251,115200*1F\r\n";
static constexpr char* set_mtk_only_gprmc = "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n";
