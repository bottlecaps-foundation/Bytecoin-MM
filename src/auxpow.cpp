// Copyright (c) 2011 Vince Durham
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.
#include "script.h"
<<<<<<< HEAD
#include "main.h"
=======
>>>>>>> 1ce1ec0... first test of merged mining patch
#include "auxpow.h"
#include "init.h"

using namespace std;
using namespace boost;

unsigned char pchMergedMiningHeader[] = { 0xfa, 0xbe, 'm', 'm' } ;

void RemoveMergedMiningHeader(vector<unsigned char>& vchAux)
{
    if (vchAux.begin() != std::search(vchAux.begin(), vchAux.end(), UBEGIN(pchMergedMiningHeader), UEND(pchMergedMiningHeader)))
        throw runtime_error("merged mining aux too short");
    vchAux.erase(vchAux.begin(), vchAux.begin() + sizeof(pchMergedMiningHeader));
}

bool CAuxPow::Check(uint256 hashAuxBlock, int nChainID)
{
    if (nIndex != 0)
        return error("AuxPow is not a generate");

<<<<<<< HEAD
    if (!fTestNet && parentBlock.GetChainID() == nChainID)
=======
    if (!fTestNet && parentBlockHeader.GetChainID() == nChainID)
>>>>>>> 1ce1ec0... first test of merged mining patch
        return error("Aux POW parent has our chain ID");

    if (vChainMerkleBranch.size() > 30)
        return error("Aux POW chain merkle branch too long");

    // Check that the chain merkle root is in the coinbase
    uint256 nRootHash = CBlock::CheckMerkleBranch(hashAuxBlock, vChainMerkleBranch, nChainIndex);
    vector<unsigned char> vchRootHash(nRootHash.begin(), nRootHash.end());
    std::reverse(vchRootHash.begin(), vchRootHash.end()); // correct endian

    // Check that we are in the parent block merkle tree
<<<<<<< HEAD
    if (CBlock::CheckMerkleBranch(GetHash(), vMerkleBranch, nIndex) != parentBlock.hashMerkleRoot)
=======
    if (CBlock::CheckMerkleBranch(GetHash(), vMerkleBranch, nIndex) != parentBlockHeader.hashMerkleRoot)
>>>>>>> 1ce1ec0... first test of merged mining patch
        return error("Aux POW merkle root incorrect");

    const CScript script = vin[0].scriptSig;

    // Check that the same work is not submitted twice to our chain.
    //

    CScript::const_iterator pcHead =
        std::search(script.begin(), script.end(), UBEGIN(pchMergedMiningHeader), UEND(pchMergedMiningHeader));

    CScript::const_iterator pc =
        std::search(script.begin(), script.end(), vchRootHash.begin(), vchRootHash.end());

    if (pc == script.end())
        return error("Aux POW missing chain merkle root in parent coinbase");

    if (pcHead != script.end())
    {
        // Enforce only one chain merkle root by checking that a single instance of the merged
        // mining header exists just before.
        if (script.end() != std::search(pcHead + 1, script.end(), UBEGIN(pchMergedMiningHeader), UEND(pchMergedMiningHeader)))
            return error("Multiple merged mining headers in coinbase");
        if (pcHead + sizeof(pchMergedMiningHeader) != pc)
            return error("Merged mining header is not just before chain merkle root");
    }
    else
    {
        // For backward compatibility.
        // Enforce only one chain merkle root by checking that it starts early in the coinbase.
        // 8-12 bytes are enough to encode extraNonce and nBits.
        if (pc - script.begin() > 20)
            return error("Aux POW chain merkle root must start in the first 20 bytes of the parent coinbase");
    }


    // Ensure we are at a deterministic point in the merkle leaves by hashing
    // a nonce and our chain ID and comparing to the index.
    pc += vchRootHash.size();
    if (script.end() - pc < 8)
        return error("Aux POW missing chain merkle tree size and nonce in parent coinbase");

    int nSize;
    memcpy(&nSize, &pc[0], 4);
    if (nSize != (1 << vChainMerkleBranch.size()))
        return error("Aux POW merkle branch size does not match parent coinbase");

    int nNonce;
    memcpy(&nNonce, &pc[4], 4);

    // Choose a pseudo-random slot in the chain merkle tree
    // but have it be fixed for a size/nonce/chain combination.
    //
    // This prevents the same work from being used twice for the
    // same chain while reducing the chance that two chains clash
    // for the same slot.
    unsigned int rand = nNonce;
    rand = rand * 1103515245 + 12345;
    rand += nChainID;
    rand = rand * 1103515245 + 12345;

    if (nChainIndex != (rand % nSize))
        return error("Aux POW wrong index");

    return true;
}

<<<<<<< HEAD
CScript MakeCoinbaseWithAux(unsigned int nHeight, unsigned int nBits, unsigned int nExtraNonce, vector<unsigned char>& vchAux)
=======
CScript MakeCoinbaseWithAux(unsigned int nBits, unsigned int nExtraNonce, vector<unsigned char>& vchAux)
>>>>>>> 1ce1ec0... first test of merged mining patch
{
    vector<unsigned char> vchAuxWithHeader(UBEGIN(pchMergedMiningHeader), UEND(pchMergedMiningHeader));
    vchAuxWithHeader.insert(vchAuxWithHeader.end(), vchAux.begin(), vchAux.end());

    // Push OP_2 just in case we want versioning later
<<<<<<< HEAD
    return CScript() << nHeight << nBits << nExtraNonce << OP_2 << vchAuxWithHeader;
=======
    return CScript() << nBits << nExtraNonce << OP_2 << vchAuxWithHeader;
>>>>>>> 1ce1ec0... first test of merged mining patch
}


void IncrementExtraNonceWithAux(CBlock* pblock, CBlockIndex* pindexPrev, unsigned int& nExtraNonce, vector<unsigned char>& vchAux)
{
<<<<<<< HEAD
    vector<unsigned char> vchAuxWithHeader(UBEGIN(pchMergedMiningHeader), UEND(pchMergedMiningHeader));
    vchAuxWithHeader.insert(vchAuxWithHeader.end(), vchAux.begin(), vchAux.end());

=======
>>>>>>> 1ce1ec0... first test of merged mining patch
    // Update nExtraNonce
    static uint256 hashPrevBlock;
    if (hashPrevBlock != pblock->hashPrevBlock)
    {
        nExtraNonce = 0;
        hashPrevBlock = pblock->hashPrevBlock;
    }
    ++nExtraNonce;
<<<<<<< HEAD
    unsigned int nHeight = pindexPrev->nHeight+1; // Height first in coinbase required for block.version=2
    pblock->vtx[0].vin[0].scriptSig = MakeCoinbaseWithAux(nHeight, pblock->nBits, nExtraNonce, vchAux);
    assert(pblock->vtx[0].vin[0].scriptSig.size() <= 100);

    pblock->hashMerkleRoot = pblock->BuildMerkleTree();
}

template
int ReadWriteAuxPow(ser_streamplaceholder& s, const boost::shared_ptr<CAuxPow>& auxpow, int nType, int nVersion, CSerActionGetSerializeSize ser_action);
=======

    pblock->vtx[0].vin[0].scriptSig = MakeCoinbaseWithAux(pblock->nBits, nExtraNonce, vchAux);
    pblock->hashMerkleRoot = pblock->BuildMerkleTree();
}


>>>>>>> 1ce1ec0... first test of merged mining patch
