//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2002-2012 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//=============================================================================

#ifndef __NOTE_H__
#define __NOTE_H__

/**
 \file
 Definition of classes Note and NoteHead.
*/

#include "element.h"
#include "symbol.h"
#include "durationtype.h"
#include "noteevent.h"
#include "pitchspelling.h"

class QPainter;

namespace Ms {

class Tie;
class Chord;
class NoteEvent;
class Text;
class Score;
class Sym;
class MuseScoreView;
class Bend;
class AccidentalState;
class Accidental;
class NoteDot;
class Spanner;
class StaffTypeTablature;
enum class SymId;

//---------------------------------------------------------
//   NoteHeadGroup
//---------------------------------------------------------

enum class NoteHeadGroup {
      HEAD_NORMAL = 0,
      HEAD_CROSS,
      HEAD_DIAMOND,
      HEAD_TRIANGLE,
      HEAD_MI,
      HEAD_SLASH,
      HEAD_XCIRCLE,
      HEAD_DO,
      HEAD_RE,
      HEAD_FA,
      HEAD_LA,
      HEAD_TI,
      HEAD_SOL,
      HEAD_BREVIS_ALT,
      HEAD_GROUPS,
      HEAD_INVALID = -1
      };

//---------------------------------------------------------
//   NoteHeadType
//---------------------------------------------------------

enum class NoteHeadType {
      HEAD_AUTO = -1, HEAD_WHOLE = 0, HEAD_HALF = 1, HEAD_QUARTER = 2,
      HEAD_BREVIS = 3,
      HEAD_TYPES
      };

//---------------------------------------------------------
//   NoteVal
//    helper structure
//---------------------------------------------------------

struct NoteVal {
      int pitch = -1;
      int tpc = INVALID_TPC;
      int fret = FRET_NONE;
      int string = STRING_NONE;
      NoteHeadGroup headGroup = NoteHeadGroup::HEAD_NORMAL;

      NoteVal() {}
      NoteVal(int p) : pitch(p) {}
      };

//---------------------------------------------------------
//   @@ NoteHead
//---------------------------------------------------------

class NoteHead : public Symbol {
      Q_OBJECT

   public:
      NoteHead(Score* s) : Symbol(s) {}
      NoteHead &operator=(const NoteHead&);
      virtual NoteHead* clone() const  { return new NoteHead(*this); }
      virtual ElementType type() const { return NOTEHEAD; }

      virtual void write(Xml& xml) const;

      NoteHeadGroup headGroup() const;
      };

//---------------------------------------------------------------------------------------
//   @@ Note
///  Graphic representation of a note.
//
//   @P subchannel        int   midi subchannel (for midi articulation) (read only)
//   @P line              int   notehead position (read only)
//   @P fret              int   fret number in tablature
//   @P string            int   string number in tablature
//   @P tpc               int   tonal pitch class
//   @P pitch             int   midi pitch
//   @P ppitch            int   actual played midi pitch (honoring ottavas) (read only)
//   @P ghost             bool  ghost note (guitar: death note)
//   @P hidden            bool  hidden, not played note (read only)
//   @P mirror            bool  mirror note head on x axis (read only)
//   @P small             bool  small note head
//   @P play              bool  play note
//   @P tuning            qreal tuning offset in cent
//   @P veloType          enum  OFFSET_VAL, USER_VAL
//   @P veloOffset        int
//   @P userMirror        enum DH_AUTO, DH_LEFT, DH_RIGHT
//   @P userDotPosition   enum AUTO, UP, DOWN
//   @P headGroup         enum HEAD_NORMAL, HEAD_CROSS, HEAD_DIAMOND, HEAD_TRIANGLE, HEAD_MI, HEAD_SLASH, HEAD_XCIRCLE, HEAD_DO, HEAD_RE, HEAD_FA, HEAD_LA, HEAD_TI, HEAD_SOL, HEAD_BREVIS_ALT
//   @P headType          enum HEAD_AUTO, HEAD_WHOLE, HEAD_HALF, HEAD_QUARTER, HEAD_BREVIS
//   @P elements          array[Element] list of elements attached to note head
//---------------------------------------------------------------------------------------

class Note : public Element {
      Q_OBJECT
      Q_PROPERTY(int subchannel                READ subchannel)
      Q_PROPERTY(int line                      READ line)
      Q_PROPERTY(int fret                      READ fret              WRITE undoSetFret)
      Q_PROPERTY(int string                    READ string            WRITE undoSetString)
      Q_PROPERTY(int tpc1                      READ tpc1              WRITE undoSetTpc1)
      Q_PROPERTY(int tpc2                      READ tpc2              WRITE undoSetTpc2)
      Q_PROPERTY(int pitch                     READ pitch             WRITE undoSetPitch)
      Q_PROPERTY(int ppitch                    READ ppitch)
      Q_PROPERTY(bool ghost                    READ ghost             WRITE undoSetGhost)
      Q_PROPERTY(bool hidden                   READ hidden)
      Q_PROPERTY(bool mirror                   READ mirror)
      Q_PROPERTY(bool small                    READ small             WRITE undoSetSmall)
      Q_PROPERTY(bool play                     READ play              WRITE undoSetPlay)
      Q_PROPERTY(qreal tuning                  READ tuning            WRITE undoSetTuning)
      Q_PROPERTY(Ms::MScore::ValueType veloType    READ veloType      WRITE undoSetVeloType)
      Q_PROPERTY(int veloOffset                READ veloOffset        WRITE undoSetVeloOffset)
      Q_PROPERTY(Ms::MScore::DirectionH userMirror READ userMirror    WRITE undoSetUserMirror)
      Q_PROPERTY(Ms::MScore::Direction userDotPosition      READ userDotPosition   WRITE undoSetUserDotPosition)
      Q_PROPERTY(NoteHeadGroup     headGroup   READ headGroup         WRITE undoSetHeadGroup)
      Q_PROPERTY(NoteHeadType      headType    READ headType          WRITE undoSetHeadType)
      Q_PROPERTY(QQmlListProperty<Element> elements  READ qmlElements)

      Q_ENUMS(NoteHeadGroup)
      Q_ENUMS(NoteHeadType)

      int _subchannel;        ///< articulation
      int _line;              ///< y-Position; 0 - top line.
      int _fret;              ///< for tablature view
      int _string;
      mutable int _tpc[2];    ///< tonal pitch class  (concert/transposing)
      mutable int _pitch;     ///< Note pitch as midi value (0 - 127).

      bool _ghost;            ///< ghost note (guitar: death note)
      bool _hidden;           ///< markes this note as the hidden one if there are
                              ///< overlapping notes; hidden notes are not played
                              ///< and heads + accidentals are not shown
      bool _dotsHidden;       ///< dots of hidden notes are hidden too
                              ///< except if only one note is dotted
      bool _fretConflict;     ///< used by TAB staves to mark a fretting conflict:
                              ///< two or mor enotes on the same string

      bool dragMode;
      bool _mirror;           ///< True if note is mirrored at stem.
      bool _small;
      bool _play;             // note is not played if false
      mutable bool _mark;     // for use in sequencer

      NoteHeadGroup _headGroup;
      NoteHeadType _headType;

      MScore::ValueType _veloType;
      short int _veloOffset; ///< velocity user offset in percent, or absolute velocity for this note

      qreal _tuning;         ///< pitch offset in cent, playable only by internal synthesizer

      MScore::DirectionH _userMirror;     ///< user override of mirror
      MScore::Direction _userDotPosition; ///< user override of dot position

      Accidental* _accidental;

      ElementList _el;        ///< fingering, other text, symbols or images
      Tie* _tieFor;
      Tie* _tieBack;

      NoteDot* _dots[MAX_DOTS];

      NoteEventList _playEvents;

      int _lineOffset;        ///< Used during mouse dragging.
      QList<Spanner*> _spannerFor;
      QList<Spanner*> _spannerBack;

      virtual QRectF drag(EditData*) override;
      void endDrag();
      void endEdit();
      void addSpanner(Spanner*);
      void removeSpanner(Spanner*);
      int concertPitchIdx() const;
      void updateRelLine();

   public:
      Note(Score* s = 0);
      Note(const Note&);
      Note& operator=(const Note&);
      ~Note();
      Note* clone() const      { return new Note(*this); }
      ElementType type() const { return NOTE; }

      virtual qreal mag() const;

      QPointF pagePos() const;      ///< position in page coordinates
      QPointF canvasPos() const;    ///< position in page coordinates
      void layout();
      void layout2();
      void layout10(AccidentalState*);
      void scanElements(void* data, void (*func)(void*, Element*), bool all=true);
      void setTrack(int val);

      int playTicks() const;

      qreal headWidth() const;
      qreal headHeight() const;
      qreal tabHeadWidth(StaffTypeTablature* tab = 0) const;
      qreal tabHeadHeight(StaffTypeTablature* tab = 0) const;
      QPointF attach() const;

      SymId noteHead() const;
      NoteHeadGroup headGroup() const     { return _headGroup; }
      NoteHeadType headType() const       { return _headType;  }
      void setHeadGroup(NoteHeadGroup val);
      void setHeadType(NoteHeadType t);

      void setPitch(int val);
      void undoSetPitch(int val);
      void setPitch(int pitch, int tpc1, int tpc2);
      int pitch() const                   { return _pitch;    }
      int ppitch() const;           ///< playback pitch
      int epitch() const;           ///< effective pitch
      qreal tuning() const                { return _tuning;   }
      void setTuning(qreal v)             { _tuning = v;      }
      void undoSetTpc(int v);
      int transposition() const;

      int tpc() const;
      int tpc1() const            { return _tpc[0]; }     // non transposed tpc
      int tpc2() const            { return _tpc[1]; }     // transposed tpc

      void setTpc(int v);
      void setTpc1(int v)         { _tpc[0] = v; }
      void setTpc2(int v)         { _tpc[1] = v; }
      void setTpcFromPitch();
      int tpc1default(int pitch) const;
      int tpc2default(int pitch) const;
      void undoSetTpc1(int tpc)      { undoChangeProperty(P_TPC1, tpc); }
      void undoSetTpc2(int tpc)      { undoChangeProperty(P_TPC2, tpc); }
      int transposeTpc(int tpc);

      Q_INVOKABLE Ms::Accidental* accidental() const    { return _accidental; }
      void setAccidental(Accidental* a)   { _accidental = a;    }

      int line() const                { return _line + _lineOffset;   }
      void setLine(int n);

      int fret() const                { return _fret;   }
      void setFret(int val)           { _fret = val;    }
      int string() const              { return _string; }
      void setString(int val);
      bool ghost() const              { return _ghost;  }
      void setGhost(bool val)         { _ghost = val;   }
      bool fretConflict() const       { return _fretConflict; }
      void setFretConflict(bool val)  { _fretConflict = val; }

      virtual void add(Element*);
      virtual void remove(Element*);

      bool mirror() const             { return _mirror;  }
      void setMirror(bool val)        { _mirror = val;   }

      bool small() const              { return _small;   }
      void setSmall(bool val);

      bool play() const               { return _play;    }
      void setPlay(bool val)          { _play = val;     }

      Q_INVOKABLE Ms::Tie* tieFor() const  { return _tieFor;  }
      Q_INVOKABLE Ms::Tie* tieBack() const { return _tieBack; }
      void setTieFor(Tie* t)          { _tieFor = t;     }
      void setTieBack(Tie* t)         { _tieBack = t;    }

      Chord* chord() const            { return (Chord*)parent(); }
      void setChord(Chord* a)         { setParent((Element*)a);  }

      void draw(QPainter*) const;

      void read(XmlReader&);
      void write(Xml& xml) const;

      bool acceptDrop(MuseScoreView*, const QPointF&, Element*) const;
      Element* drop(const DropData&);

      bool hidden() const                       { return _hidden; }
      void setHidden(bool val)                  { _hidden = val;  }
      bool dotsHidden() const                   { return _dotsHidden; }
      void setDotsHidden(bool val)              { _dotsHidden = val;  }

      NoteType noteType() const;

      ElementList el()                            { return _el; }
      const ElementList el() const                { return _el; }
      QQmlListProperty<Ms::Element> qmlElements() { return QQmlListProperty<Ms::Element>(this, _el); }

      int subchannel() const                    { return _subchannel; }
      void setSubchannel(int val)               { _subchannel = val;  }

      MScore::DirectionH userMirror() const     { return _userMirror; }
      void setUserMirror(MScore::DirectionH d)  { _userMirror = d; }

      MScore::Direction userDotPosition() const       { return _userDotPosition; }
      void setUserDotPosition(MScore::Direction d)    { _userDotPosition = d;    }
      bool dotIsUp() const;               // actual dot position

      void reset();

      MScore::ValueType veloType() const    { return _veloType;          }
      void setVeloType(MScore::ValueType v) { _veloType = v;             }
      int veloOffset() const                { return _veloOffset;        }
      void setVeloOffset(int v)             { _veloOffset = v;           }

      void setOnTimeOffset(int v);
      void setOffTimeOffset(int v);

      int customizeVelocity(int velo) const;
      Q_INVOKABLE Ms::NoteDot* dot(int n)       { return _dots[n];           }
      void updateAccidental(AccidentalState*);
      void updateLine();
      void setNval(NoteVal);
      NoteEventList& playEvents()                { return _playEvents; }
      const NoteEventList& playEvents() const    { return _playEvents; }
      NoteEvent* noteEvent(int idx)              { return &_playEvents[idx]; }
      void setPlayEvents(const NoteEventList& l) { _playEvents = l;    }

      QList<Spanner*> spannerFor() const         { return _spannerFor;         }
      QList<Spanner*> spannerBack() const        { return _spannerBack;        }

      void addSpannerBack(Spanner* e)            { _spannerBack.push_back(e);  }
      bool removeSpannerBack(Spanner* e)         { return _spannerBack.removeOne(e); }
      void addSpannerFor(Spanner* e)             { _spannerFor.push_back(e);         }
      bool removeSpannerFor(Spanner* e)          { return _spannerFor.removeOne(e);  }

      void transposeDiatonic(int interval, bool keepAlterations, bool useDoubleAccidentals);

      void undoSetFret(int);
      void undoSetString(int);
      void undoSetGhost(bool);
      void undoSetMirror(bool);
      void undoSetSmall(bool);
      void undoSetPlay(bool);
      void undoSetTuning(qreal);
      void undoSetVeloType(MScore::ValueType);
      void undoSetVeloOffset(int);
      void undoSetOnTimeUserOffset(int);
      void undoSetOffTimeUserOffset(int);
      void undoSetUserMirror(MScore::DirectionH);
      void undoSetUserDotPosition(MScore::Direction);
      void undoSetHeadGroup(NoteHeadGroup);
      void undoSetHeadType(NoteHeadType);

      virtual QVariant getProperty(P_ID propertyId) const;
      virtual bool setProperty(P_ID propertyId, const QVariant&);
      virtual QVariant propertyDefault(P_ID) const;

      bool mark() const               { return _mark;   }
      void setMark(bool v) const      { _mark = v;   }
      virtual void setScore(Score* s);
      void setDotY(MScore::Direction);

      static SymId noteHead(int direction, NoteHeadGroup, NoteHeadType);
      };

// extern const SymId noteHeads[2][int(NoteHeadGroup::HEAD_GROUPS)][int(NoteHeadType::HEAD_TYPES)];


}     // namespace Ms

Q_DECLARE_METATYPE(Ms::NoteHeadGroup)
Q_DECLARE_METATYPE(Ms::NoteHeadType)

#endif

