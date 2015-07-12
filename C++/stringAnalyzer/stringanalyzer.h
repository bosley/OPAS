#ifndef STRINGANALYZER_H
#define STRINGANALYZER_H

#include <map>
#include <iostream>

#define DIFFERENCE_THRESHOLD  0.25
#define MAX_SANALYZER_DATASETS 100



/*


    First make structure list of datasets, then load into analyzer

*/


namespace sanalyzer
{

    typedef struct dataset
    {
        int size;
        std::string dataLabel;
        std::map<int, std::string> dataSet;

    } _dataSet;


    class stringAnalyzer
    {
        public:

            stringAnalyzer();

            bool loadDataSets( int,  _dataSet[] );

            std::string ofDataSet( std::string );

            bool compair(std::string, std::string);

        private:

            int numberOfDataSets;

            _dataSet allDataSets[MAX_SANALYZER_DATASETS];

            bool scanner( std::string, std::string );
    };

}
#endif // STRINGANALYZER_H
