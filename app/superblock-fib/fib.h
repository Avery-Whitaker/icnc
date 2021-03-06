//********************************************************************************
// Copyright (c) 2007-2014 Intel Corporation. All rights reserved.              **
//                                                                              **
// Redistribution and use in source and binary forms, with or without           **
// modification, are permitted provided that the following conditions are met:  **
//   * Redistributions of source code must retain the above copyright notice,   **
//     this list of conditions and the following disclaimer.                    **
//   * Redistributions in binary form must reproduce the above copyright        **
//     notice, this list of conditions and the following disclaimer in the      **
//     documentation and/or other materials provided with the distribution.     **
//   * Neither the name of Intel Corporation nor the names of its contributors  **
//     may be used to endorse or promote products derived from this software    **
//     without specific prior written permission.                               **
//                                                                              **
// This software is provided by the copyright holders and contributors "as is"  **
// and any express or implied warranties, including, but not limited to, the    **
// implied warranties of merchantability and fitness for a particular purpose   **
// are disclaimed. In no event shall the copyright owner or contributors be     **
// liable for any direct, indirect, incidental, special, exemplary, or          **
// consequential damages (including, but not limited to, procurement of         **
// substitute goods or services; loss of use, data, or profits; or business     **
// interruption) however caused and on any theory of liability, whether in      **
// contract, strict liability, or tort (including negligence or otherwise)      **
// arising in any way out of the use of this software, even if advised of       **
// the possibility of such damage.                                              **
//********************************************************************************
#ifndef fib_H_ALREADY_INCLUDED
#define fib_H_ALREADY_INCLUDED

#include <cnc/cnc.h>
#include <cnc/debug.h>

#define CHUNK_SIZE 7

// Forward declaration of the context class (also known as graph)
struct fib_context;
struct superblock_manager;

// The step classes

struct fib_step
{
    int execute( const int & t, fib_context & c ) const;
};

struct super_fib_step
{
    int execute(const int tag, fib_context &ctx) const;
};

// The context class
struct fib_context : public CnC::context< fib_context >
{
    int n;
    // step collections
    CnC::step_collection< fib_step > m_steps;
    // Item collections
    CnC::item_collection< int, fib_type > m_fibs;
    // Tag collections
    CnC::tag_collection< int > m_tags;

    superblock_manager *m_super_manager;

    // The context class constructor
    fib_context(int);
};

struct superblock_manager : CnC::tag_collection< int >::callback_type
{
    CnC::step_collection<super_fib_step> m_sstep;
    CnC::tag_collection<int> m_stags;
    fib_context &context;

    superblock_manager(fib_context &context) : context(context), m_sstep(context, "sstep"), m_stags(context, "stags") {
        m_stags.prescribes(m_sstep, context);
        m_sstep.consumes(context.m_fibs);
        m_sstep.produces(context.m_fibs);
    }

    virtual bool on_put( const int & tag) override {
        if(tag >= CHUNK_SIZE && tag < context.n - context.n % CHUNK_SIZE) {
            if( tag % CHUNK_SIZE == 0) {
               // std::cout << "do chunk tag" << tag << std::endl;
                m_stags.put(tag);
            }
            //std::cout << "dont exec tag" << tag << std::endl;
            return true;
        }
       // std::cout << "do exec tag" << tag << std::endl;
        return false;
    }
};


int super_fib_step::execute(const int tag, fib_context &ctx) const
{
    fib_type mem[CHUNK_SIZE + 3];
    ctx.m_fibs.get(tag - 2, mem[0]);
    ctx.m_fibs.get(tag - 1, mem[1]);

    for(int i = 2; i < CHUNK_SIZE + 2; i++)
       mem[i] = mem[i-1] + mem[i-2];

    ctx.m_fibs.put(tag + CHUNK_SIZE - 2, mem[CHUNK_SIZE]);
    ctx.m_fibs.put(tag + CHUNK_SIZE - 1, mem[CHUNK_SIZE + 1]);

   // std::cout << "superblock: " << tag << std::endl;
    return CnC::CNC_Success;
}

fib_context::fib_context(int n)
        : n(n),
          CnC::context< fib_context >(),
        // Initialize each step collection
          m_steps( *this, "fib_step" ),
        // Initialize each tag collection
          m_fibs( *this, "fibs" ),
        // Initialize each item collection
          m_tags( *this, "tags" )
{
    m_super_manager = new superblock_manager(*this);
    m_tags.on_put(m_super_manager);
    // prescribe compute steps with this (context) as argument
    m_tags.prescribes( m_steps, *this );
    // step consumes m_fibs
    m_steps.consumes( m_fibs );
    // step also produces m_fibs
    m_steps.produces( m_fibs );
}

#endif // fib_H_ALREADY_INCLUDED
