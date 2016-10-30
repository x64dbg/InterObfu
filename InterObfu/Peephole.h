#pragma once

#include "Pattern.h"
#include "Convert.h"
#include "aho-corasick.h"
#include "Assemble.h"

class Peephole
{
public:
    explicit Peephole(std::vector<Pattern> & patterns)
        : mPatterns(patterns)
    {
        preprocessTrie();
    }

    bool Optimize(const std::vector<Instruction> & in, std::vector<Instruction> & out)
    {
        out.clear();
        if(in.empty()) //empty input cannot be optimized
            return false;

        //Use Aho-Corasick to find all mnemonic matches in O(n + r) with n = in.size(), r = results.size()
        auto results = mTrie.parse_text(preprocessIn(in));

        if(results.empty()) //no patterns found = not optimized
            return false;

        /*puts("results (unfiltered):");
        for(auto & r : results)
            printf("%u:%d-%d \"%s\"\n", r.get_index(), int(r.get_start()), int(r.get_end()), prettyPrint(r.get_keyword()).c_str());
        puts("");*/

        //This creates a map from (match start) -> (result index).
        //Before considering a result it is checked with a predicate.
        //On conflicting (match start) we use a predicate.

        auto validPredicate = [this, &in](const aho_corasick::emit<Mnem> & result) //is r a valid result?
        {
            //Check the wildcard pattern in O(m) with m = r.size()
            return mPatterns[result.get_index()].Match(in, result.get_start());
        };

        auto betterPredicate = [this](const aho_corasick::emit<Mnem> & a, const aho_corasick::emit<Mnem> & b) //is a better than b?
        {
            //make sure to first compare on equality and if not, return the compare

            if(a.size() != b.size()) //longer patterns are always better
                return a.size() > b.size();

            if(a.get_start() != b.get_start()) //patterns that start earlier are better
                return a.get_start() < b.get_start();

            const auto & pa = mPatterns.at(a.get_index());
            const auto & pb = mPatterns.at(b.get_index());
            if(pa.priority != pb.priority) //patterns with higher priority are better
                return pa.priority > pb.priority;

            if(pa.repls.size() != pb.repls.size()) //shorter replacements are better
                return pa.repls.size() < pb.repls.size();

            return false; //both patterns are equal
        };

        //Drop fake matches (mnemonic only, not operands) in O(r * x) with r = results.size(), x = max(results[i].size())
        std::unordered_map<size_t, size_t> best;
        best.reserve(results.size());
        for(size_t i = 0; i < results.size(); i++)
        {
            const auto & result = results.at(i);
            if(!validPredicate(result)) //skip invalid results
                continue;
            auto found = best.find(result.get_start());
            if(found == best.end()) //add the current result if not found
                best[result.get_start()] = i;
            else if(betterPredicate(result, results[found->second])) //replace if the current result better
                found->second = i;
        }

        if(best.empty()) //no patterns found = not optimized
            return false;

        /*puts("results (filtered):");
        for(const auto & it : best)
        {
            const auto & r = results[it.second];
            printf("%u:%d-%d \"%s\"\n", r.get_index(), int(r.get_start()), int(r.get_end()), prettyPrint(r.get_keyword()).c_str());
        }
        puts("");*/

        //Construct the output in O(i) with i = in.size()
        //TODO: if patterns overlap the first is taken and the overlaps are ignored
        out.reserve(in.size());
        for(size_t i = 0; i < in.size();)
        {
            auto found = best.find(i);
            if(found != best.end()) //current instruction is the start of a match
            {
                const auto & result = results.at(found->second);
                auto & pattern = mPatterns[result.get_index()];
                pattern.Match(in, i); //match to get the correct state
                std::vector<Instruction> product;
                if(!pattern.Produce(product))
                    __debugbreak(); //pattern replacement production failure (invalid pattern?)
                for(const auto & p : product)
                    out.push_back(p);
                i += pattern.instrs.size();
            }
            else //current instruction is not the start of a match
                out.push_back(in.at(i++));
        }

        return true;
    }

private:
    using Mnem = Opcode::Mnemonics;

    std::vector<Pattern> & mPatterns;
    aho_corasick::basic_trie<Mnem> mTrie;

    static std::string prettyPrint(const std::basic_string<Mnem> & keyword)
    {
        std::string result;
        for(auto & mnem : keyword)
        {
            if(!result.empty())
                result.push_back(';');
            result.append(Converter::ins2str(mnem));
        }
        return result;
    }

    static std::basic_string<Mnem> preprocessIn(const std::vector<Instruction> & in)
    {
        std::basic_string<Mnem> text;
        text.resize(in.size());
        for(size_t i = 0; i < in.size(); i++)
        {
            if(in[i].opcode.mnem == X86_INS_INVALID)
                __debugbreak(); //wildcards are not allowed
            text[i] = in[i].opcode.mnem;
        }
        return text;
    }

    void preprocessTrie()
    {
        //Construct the Aho-Corasick trie in O(m) with m = mPatterns.size()
        std::basic_string<Mnem> keyword;
        for(const auto & pattern : mPatterns)
        {
            keyword.clear();
            for(const auto & instr : pattern.instrs)
            {
                if(instr.opcode.mnem == X86_INS_INVALID)
                    __debugbreak(); //wildcards are not allowed
                keyword.push_back(instr.opcode.mnem);
            }
            mTrie.insert(keyword);
        }
    }
};
