#ifndef COORDINATES_HPP
#define COORDINATES_HPP

struct coordinates {
    int latitude, longitude;
    coordinates(int lat, int lon) : latitude(lat), longitude(lon) {}
    coordinates() : latitude(0), longitude(0) {}
};


#endif // COORDINATES_HPP