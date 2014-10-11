/*
  Copyright 2014 Andreas Lauser

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
#ifndef OPM_PARSER_LOG_HPP
#define OPM_PARSER_LOG_HPP

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <memory>

namespace Opm {
/*!
 * \brief Provides a simple sytem for log message which are found by the
 *        Parser/Deck/EclipseState classes during processing the deck.
 */
class ParserLog {
public:
    enum MessageType {
        Note = 0x01,
        Warning = 0x02,
        Error = 0x04,

        AllTypes = 0xff
    };

    ParserLog();
    ParserLog(std::ostream* os);

    void setOutStream(std::ostream* os);

    size_t size() const;
    size_t numErrors() const;
    size_t numWarnings() const;
    size_t numNotes() const;

    void addMessage(const std::string& fileName,
                    int lineNumber,
                    MessageType messageType,
                    const std::string& description);

    void addNote(const std::string& fileName,
                 int lineNumber,
                 const std::string& description);
    void addWarning(const std::string& fileName,
                    int lineNumber,
                    const std::string& description);
    void addError(const std::string& fileName,
                  int lineNumber,
                  const std::string& description);

    const std::string& getFileName(size_t msgIdx) const;
    int getLineNumber(size_t msgIdx) const;
    int getMessageType(size_t msgIdx) const;
    const std::string& getDescription(size_t msgIdx) const;

    /*!
     * \brief This method takes the information provided by the methods above and returns
     *        them in a fully-formatted string.
     *
     * It is thus covenience method to convert a log message into a GCC-like format,
     * e.g. a "Note" message triggered by the file "SPE1DECK.DATA" on line 15 which says
     * that no grid can be constructed would yield:
     *
     * SPE1DECK.DATA:15:note: No grid found.
     */
    const std::string getFormattedMessage(size_t msgIdx) const;

    /*!
     * \brief Print all issues described in a log object to a std::ostream.
     *
     * This is just another convenience method...
     */
    void printAll(std::ostream &os = std::cerr,
                  size_t enabledTypes = AllTypes) const;

private:
    typedef std::tuple</*file=*/std::string,
                       /*line=*/int,
                       /*MessageType=*/int,
                       std::string> MessageTuple;
    std::vector<MessageTuple> m_messages;

    size_t m_numErrors;
    size_t m_numWarnings;
    size_t m_numNotes;

    mutable std::ostream* m_outStream;
};

typedef std::shared_ptr<ParserLog> ParserLogPtr;
typedef std::shared_ptr<const ParserLog> ParserLogConstPtr;
} // namespace Opm

#endif
