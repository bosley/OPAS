#ifndef FBOOL_H
#define FBOOL_H

/*

	Josh Allen Bosley
	09/26/14
*/

namespace bosley
{

typedef struct fbool
{
    // Constructors
    fbool();
    fbool(bool);
    fbool(double);

    // Set / Get toBool
    void set(bool);
    void set(double);
    double get() const;
    bool toBool() const;

    // Operator Overloads
    fbool operator&&(fbool);
    fbool operator&&(bool);
    fbool operator||(fbool);
    fbool operator||(bool);
    fbool operator!();

    static bool randomized;
    double probability;

} fbool;

// More Operators
fbool operator&&(bool, fbool);
fbool operator||(bool, fbool);

}
#endif // FBOOL_H
