/*
  Copyright 2015 Statoil ASA.

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <stdexcept>
#include <iostream>
#include <cstdio>

#if HAVE_DYNAMIC_BOOST_TEST
#define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MODULE InputKeywordTests

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


#include <opm/parser/eclipse/Generator/KeywordLoader.hpp>
#include <opm/parser/eclipse/Parser/ParserKeyword.hpp>
#include <opm/parser/eclipse/Parser/ParserRecord.hpp>

inline std::string prefix() {
    return boost::unit_test::framework::master_test_suite().argv[1];
}

BOOST_AUTO_TEST_CASE(EmptyKeywordLoader) {
    Opm::KeywordLoader loader(false);

    BOOST_CHECK_EQUAL( false , loader.hasKeyword("NO"));
    BOOST_CHECK_EQUAL( 0U , loader.size() );
    BOOST_CHECK_THROW( loader.getKeyword("NO") , std::invalid_argument);
}



BOOST_AUTO_TEST_CASE(LoadKeyword) {
    Opm::KeywordLoader loader(false);

    BOOST_CHECK_THROW( loader.loadKeyword("does/not/exists") , std::invalid_argument );
    BOOST_CHECK_THROW( loader.loadKeyword(prefix() + "invalid.json") , std::invalid_argument);
    BOOST_CHECK_THROW( loader.loadKeyword(prefix() + "PORO-invalid") , std::invalid_argument);

    loader.loadKeyword(prefix() + "PORO.json");
    loader.loadKeyword(prefix() + "PORO.json");

    BOOST_CHECK_EQUAL( true , loader.hasKeyword("PORO"));
    BOOST_CHECK_EQUAL( 1U , loader.size() );

    loader.getKeyword("PORO");
}



BOOST_AUTO_TEST_CASE(LoadKeywordDirectory) {
    Opm::KeywordLoader loader(false);
    BOOST_CHECK_THROW( loader.loadKeywordDirectory("does/not/exists") , std::invalid_argument );
    BOOST_CHECK_THROW( loader.loadKeywordDirectory(prefix() + "invalid.json") , std::invalid_argument);

    BOOST_CHECK_EQUAL( 4 , loader.loadKeywordDirectory( prefix() + "loader/001_ECLIPSE100"));
    BOOST_CHECK( loader.hasKeyword("ADDREG") );
    BOOST_CHECK( loader.hasKeyword("ACTNUM") );
    BOOST_CHECK( loader.hasKeyword("BOX") );
    BOOST_CHECK( loader.hasKeyword("BLOCK_PROBE") );
    {
        auto kw = loader.getKeyword("ADDREG");
        auto record = kw->getRecord(0);
        BOOST_CHECK( !record.hasItem("REGION_NUMBER") );
    }
}


BOOST_AUTO_TEST_CASE(DirectorySort) {
    BOOST_CHECK_THROW( Opm::KeywordLoader::sortSubdirectories( prefix() + "loader/ZCORN") , std::invalid_argument );
    std::vector<std::string> dir_list = Opm::KeywordLoader::sortSubdirectories( prefix() + "loader");

    namespace fs = boost::filesystem;

    BOOST_CHECK_EQUAL( 2U , dir_list.size());
    BOOST_CHECK_EQUAL( fs::path( dir_list[0] ), fs::path( prefix() + "loader/001_ECLIPSE100" ) );
    BOOST_CHECK_EQUAL( fs::path( dir_list[1] ), fs::path( prefix() + "loader/002_ECLIPSE300" ) );
}


BOOST_AUTO_TEST_CASE(BigLoad) {
    Opm::KeywordLoader loader(false);
    BOOST_CHECK_THROW( loader.loadMultipleKeywordDirectories("does/not/exists") , std::invalid_argument );
    BOOST_CHECK_THROW( loader.loadMultipleKeywordDirectories(prefix() + "invalid.json") , std::invalid_argument);

    loader.loadMultipleKeywordDirectories(prefix() + "loader");
    BOOST_CHECK( loader.hasKeyword("EQUIL"));
    {
        auto kw = loader.getKeyword("ADDREG");
        auto record = kw->getRecord(0);
        BOOST_CHECK( record.hasItem("REGION_NUMBER"));
    }
}
