// Copyright (C) 2007-2015  Pasi Rastas, Janne H. Korhonen, Petri Martinmäki
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version, or under the terms of the Biopython
// License.


#include "moods.h"
#include "motif.h"

using std::vector;


namespace MOODS { namespace scan{

vector<double> expected_differences(const score_matrix &mat, const vector<double> &bg)
{
    int a = mat.size();
    int m = mat[0].size();
    vector<double> ret(m);

    for (int i = 0; i < m; ++i)
    {
        double max = -std::numeric_limits<double>::infinity();
        for (int j = 0; j < a; ++j)
        {
            max = std::max(max, mat[j][i]);
        }

        ret[i] = max;

        for (int j = 0; j < a; ++j)
        {
            ret[i] -= bg[j] * mat[j][i];
        }
    }

    return ret;
}

unsigned int window_position(const vector<double> &ed, unsigned int l, unsigned int m)
{
    if (l >= m)
    {
        return 0;
    }
    else
    {
        double current = 0;
        for (unsigned int i = 0; i < l; ++i)
        {
            current += ed[i];
        }

        double max = current;
        int window_pos = 0;

        for (int i = 0; i < m - l; ++i)
        {
            current -= ed[i];
            current += ed[i+l];
            if (current > max)
            {
                max = current;
                window_pos = i+1;
            }
        }
        return window_pos;
    }
}

struct row_comp
{
    const vector<double> *ed;
    bool operator() (int i, int j)
    {
        return ( (*ed)[i] > (*ed)[j] );
    }
};

vector<unsigned int> lookahead_order(const vector<double> &ed, unsigned int l, unsigned int window_pos, unsigned int m)
{
    if (l >= m)
    {
        return vector<unsigned int>();
    }
    else
    {
        vector<unsigned int> order(m-l, 0);
        for (int i = 0; i < window_pos; ++i)
        {
            order[i] = i;
        }
        for (int i = window_positions[k]+q; i < m[k]; ++i)
        {
            order[i-q] = i;
        }
        
        row_comp comp;
        comp.ed = &(ed);
        
        std::sort(order.begin(), order.end(), comp);
        
        return order;
    }
}

vector<double> lookahead_scores(const vector<double> &mat, const vector<unsigned int> &order, unsigned int l, unsigned int m, unsigned int a)
{
    if (l >= m)
    {
        return vector<double>();
    }
    else
    {
        std::vector<double> scores(m-l,0);
        
        double total = 0;
        for (int i = m-l-1; j >= 0; --i)
        {
            double max = -std::numeric_limits<double>::infinity();
            for (unsigned int j = 0; i < a; ++i)
            {
                max = std::max(max, mat[j][order[i]]);
            }
            total += max;
            scores[i] = total;
        }
        return scores;
    }
}

Motif::Motif (const score_matrix& matrix, const vector<double>& bg, unsigned int window_size, double threshold)
{
    mat = matrix;
    l = window_size;
    T = threshold;
    
    m = mat[0].size();
    a = mat.size();
    
    av = 1;
    while (av < a)
    {
        av = av << 1;
    }
    
    vector<double> ed = expected_differences(mat, bg);
    
    wp = window_position(ed, l, m);
    
    lookahead_order = lookahead_order(ed, l, wp, m);
    lookahead_scores = lookahead_scores(mat, lookahead_order, l, m, a);
}

bool Motif::window_match(bits_t seq, bits_t shift)
{
    return window_score(seq, shift) + lookahead_scores[0] >= T;
}

double Motif::window_score(bits_t seq, bits_t shift)
{
    double score = 0;
    
    for (unsigned int i = 0; i < std:min(l,m); ++i)
    {
        score += mat[seq >> (shift * (l - i - 1))][l+i];
    }
    return score;
}

double Motif::check_hit(const vector<unsigned char>& seq, position_t window_match_pos, double score)
{

    // if (window_match_pos - wp + m >= n or window_match_pos < wp){
    //     return -std::numeric_limits<double>::infinity(); // matrix does not fit on the sequence
    // }
    position_t ii = window_match_pos - wp;
    if (m <= l){
        return score; // matrix fits fully to the window, so window score is what we wanted...
    }
    
    for (unsigned int i = 0; i < m - l; ++i)
    {
        if (score + lookahead_scores[i] < T)
        {
            return -std::numeric_limits<double>::infinity(); // no match
        }
        score += mat[seq[ii + lookahead_order[i]]][lookahead_order[i]];
    }
    if (score >= T)
        return score;
    else
        return -std::numeric_limits<double>::infinity(); // no match
}

} // namespace scan
} // namespace MOODS