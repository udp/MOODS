#ifndef MOODS_SCAN_H
#define MOODS_SCAN_H

#include <string>

#include "moods.h"
#include "scanner.h"
#include "match_types.h"

namespace MOODS { namespace scan{
    


    
    std::vector<std::vector<match> > scan_dna(const std::string& seq,
                                                const std::vector<score_matrix>& matrices,
                                                const std::vector<double>& bg,
                                                const std::vector<double> thresholds,
                                                unsigned int window_size );

    std::vector<std::vector< match> > scan(const std::string& seq,
                                            const std::vector<score_matrix>& matrices,
                                            const std::vector<double>& bg,
                                            const std::vector<double> thresholds,
                                            unsigned int window_size,
                                            const std::vector<std::string>& alphabet);

    std::vector<match> naive_scan_dna(const std::string& seq, const score_matrix matrix, double threshold);
    std::vector<match> naive_scan_dna(const std::string& seq, const score_matrix matrix, double threshold, size_t a);
}}



#endif