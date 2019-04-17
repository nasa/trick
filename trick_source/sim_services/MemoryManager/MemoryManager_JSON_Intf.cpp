#include <iostream>
#include "trick/MemoryManager.hh"

void Trick::MemoryManager::write_JSON_alloc_info(std::ostream& s, ALLOC_INFO *alloc_info) {
    s << "{" << std::endl;
    if (!alloc_info->name) {
        s << "\"name\":null," << std::endl;
    } else {
        s << "\"name\":\"" << alloc_info->name << "\"," << std::endl;
    }
    s << "\"start\":\"" << alloc_info->start << "\"," << std::endl;
    s << "\"end\":\"" << alloc_info->end << "\"," << std::endl;
    s << "\"num\":\"" << alloc_info->num << "\"," << std::endl;
    s << "\"size\":\"" << alloc_info->size << "\"," << std::endl;
    s << "\"type\":\"" << trickTypeCharString(alloc_info->type, alloc_info->user_type_name) << "\"," << std::endl;
    s << "\"stcl\":" ;
    if (alloc_info->stcl == TRICK_LOCAL) {
        s << "\"TRICK_LOCAL\"," << std::endl;
    }
    if (alloc_info->stcl == TRICK_EXTERN) {
        s << "\"TRICK_EXTERN\"," << std::endl;
    }
    s << "\"language\":";
    if (alloc_info->language == Language_C  ) {
        s << "\"Language_C\"," << std::endl;
    }
    if (alloc_info->language == Language_CPP) {
        s << "\"Language_CPP\"," << std::endl;
    }
    s << "\"index\": [" ;
    for (int ii=0; ii<alloc_info->num_index; ii++) {
        if (ii != 0) {
            s << "," ;
        } 
        s << alloc_info->index[ii] ;
    }
    s << "]}" ;
}


void Trick::MemoryManager::write_JSON_alloc_list(std::ostream& s, int chunk_start, int chunk_size) {

    ALLOC_INFO_MAP::iterator pos;
    ALLOC_INFO* alloc_info;

    int size = alloc_info_map.size();
    s << "{" << std::endl;
    s << "\"alloc_total\":" << size << "," << std::endl;
    s << "\"chunk_size\":" << chunk_size << "," << std::endl;
    s << "\"chunk_start\":" << chunk_start << "," << std::endl;
    s << "\"alloc_list\":[" << std::endl;
    pos = alloc_info_map.begin();
    std::advance(pos, chunk_start);

    for (int count = 0; count < chunk_size && pos!=alloc_info_map.end() ; pos++, count++) {
        alloc_info = pos->second;
        if (count != 0) {
            s << "," << std::endl;
        } 
        write_JSON_alloc_info(s, alloc_info);
    }
    s << "]}" << std::endl;
}

