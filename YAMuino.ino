/*
   YAMuino
   Yamaha CDX-E410 IR Bridge
   http://johnlenfr.fr

   Bridge Arduino permettant de piloter un lecteur CD Yamaha CDX-E410
   sans ampli RX-E410, en relayant directement les commandes de la
   télécommande IR vers l'entrée "System Control" du lecteur.

   Principe:
   - réception d'une trame IR NEC via capteur IR
   - lecture de l'address + command
   - réémission de la trame NEC complète sur la ligne de commande CD

   Le CDX-E410 attend sur son bus "System Control" un signal NEC démodulé.
   Address / custom code Yamaha CD = 0x78

   Trame NEC émise vers le lecteur:
   - Header LOW  = 9000 us
   - Header HIGH = 4500 us
   - Bit LOW     = 560 us
   - Bit HIGH 0  = 560 us
   - Bit HIGH 1  = 1690 us
   - Envoi LSB first
   - Frame = custom, ~custom, command, ~command

   COMPILED WITH IDE v2.3.6
   with installed libs:
   - IRremote.hpp

   ------------------------------------------------------------
   Arduino UNO pin mapping
   ------------------------------------------------------------

   D0:  RX Serial
   D1:  TX Serial
   D2:  IR sensor output  -> IR_PIN
   D3:  To TRS cable TIP  -> OUT_PIN
   D4:
   D5:
   D6:
   D7:
   D8:
   D9:
   D10:
   D11:
   D12:
   D13: LED onboard

   A0:
   A1:
   A2:
   A3:
   A4: SDA
   A5: SCL

   ------------------------------------------------------------
   Hardware
   ------------------------------------------------------------

   - Arduino Uno
   - IR receiver module
   - Jack 3.5 mm TRS cable
   - Yamaha CDX-E410

   ------------------------------------------------------------
   TRS cable wiring
   ------------------------------------------------------------

   TRS plug:
   - TIP    = RX command line
   - RING   = State info line (non utilisé dans cette version)
   - SLEEVE = GND

   Wiring used in this project:
   - Arduino D3   -> R=1kOhm -> TIP
   - Arduino GND             -> SLEEVE
   - RING         -> not connected

   Note:
   - ce montage a été validé sans besoin d'un transistor
   - si instable sur un autre Arduino, ajouter une résistance série
     faible ou revenir à une sortie type collecteur ouvert

   ------------------------------------------------------------
   Yamaha NEC values (Service Manual / relevés télécommande)
   ------------------------------------------------------------

   Address / Custom:
   - YAMAHA_ADDR = 0x78

   Main CD commands:
   - STOP        = 0x01
   - PLAY        = 0x02
   - NEXT        = 0x03   // SKIP/SCAN +
   - PREV        = 0x04   // SKIP/SCAN -
   - RANDOM      = 0x07
   - TIME/INFO   = 0x0A
   - PROGRAM     = 0x0B
   - REPEAT      = 0x0C
   - PAUSE       = 0xB9

   Numeric commands:
   - 0           = 0x10
   - 1           = 0x11
   - 2           = 0x12
   - 3           = 0x13
   - 4           = 0x14
   - 5           = 0x15
   - 6           = 0x16
   - 7           = 0x17
   - 8           = 0x18
   - 9           = 0x19

   System requests:
   - POWER_ON    = 0x7E
   - STANDBY     = 0x7F
   - DIMMER_ON   = 0xFE
   - DIMMER_OFF  = 0xFF
   - VFD_OFF     = 0xFD

   Remote special case:
   - POWER key from remote sends command 0x0F
   - this sketch remaps it to:
       * 0x7E when CD is considered in standby
       * 0x7F when CD is considered awake

   ------------------------------------------------------------
   TVIX DVICO complementary remote
   ------------------------------------------------------------

   Protocol:
   - NEC

   Address:
   - TVIX_ADDR = 0xFD00

   Repeat behavior:
   - flags = 0x00 : appui normal
   - flags = 0x01 : appui maintenu / repeat

   Mapping retained:
   - POWER         -> Power ON / Standby
   - PLAY/PAUSE    -> Play / Pause / Resume
   - STOP          -> Stop
   - NEXT          -> Next track
   - RIGHT         -> Next track
   - PREVIOUS      -> Previous track
   - LEFT          -> Previous track
   - INFO          -> Time / Info
   - SHUFFLE       -> Random
   - GOTO          -> Repeat
   - SETUP         -> Program
   - TV OUT        -> Display / Dimmer / VFD off
   - 0..9          -> Direct numeric track selection

   TVIX commands received:
   - POWER      : 0x57
   - 1          : 0x19
   - 2          : 0x18
   - 3          : 0x1B
   - 4          : 0x5B
   - 5          : 0x1A
   - 6          : 0x58
   - 7          : 0x59
   - 8          : 0x5A
   - 9          : 0x14
   - 0          : 0x17
   - GOTO       : 0x15
   - INFO       : 0x55
   - PLAY/PAUSE : 0x16
   - UP         : 0x54
   - STOP       : 0x56
   - LEFT       : 0x09
   - OK         : 0x08
   - RIGHT      : 0x49
   - PREVIOUS   : 0x0A
   - DOWN       : 0x0B
   - NEXT       : 0x4B
   - VOL -      : 0x05
   - VOL +      : 0x04
   - MUTE       : 0x45
   - ZOOM       : 0x06
   - SCROLL     : 0x44
   - SUBTITLE   : 0x07
   - BOOKMARK   : 0x47
   - MENU       : 0x02
   - TITLE      : 0x40
   - AUDIO      : 0x41
   - ANGLE      : 0x42
   - SHUFFLE    : 0x01
   - SETUP      : 0x00
   - PICTURE    : 0x03
   - TV OUT     : 0x43

   ------------------------------------------------------------
   Software behavior
   ------------------------------------------------------------

   This sketch:
   - listens for NEC frames from the Yamaha remote
   - processes frames where address == 0x78
   - also processes frames from TVIX DVICO where address == 0xFD00
   - forwards Yamaha commands directly to the CD player
   - maps TVIX commands to Yamaha CD commands
   - handles POWER as a special remapped command
   - handles PLAY button as PLAY / PAUSE / RESUME depending on
     the internal software state

   Internal software states used:
   - CD_STANDBY
   - CD_STOPPED
   - CD_PLAYING
   - CD_PAUSED

   PLAY button behavior:
   - if stopped  -> send PLAY
   - if playing  -> send PAUSE
   - if paused   -> send PAUSE again to resume

   Repeat behavior:
   - Yamaha remote:
       * repeat ignored for most commands
       * repeat allowed for NEXT / PREV
   - TVIX remote:
       * repeat ignored for most commands
       * repeat allowed for NEXT / PREV / RIGHT / LEFT
       * numeric keys 0..9 are sent once only

   ------------------------------------------------------------
   Known limitations
   ------------------------------------------------------------

   - The "State info" line on the TRS ring is not read in this version
   - CD state is partially tracked in software only
   - If front-panel buttons are used, software state may become desynced
   - POWER handling is based on remote command remapping

   ------------------------------------------------------------
   Future improvements
   ------------------------------------------------------------

   TO DO:
   - lire la ligne State info sur RING pour connaître l'état réel du CD
   - synchroniser automatiquement PLAY / PAUSE / STOP / STANDBY
   - ajouter une LED d'activité
   - ajouter une résistance série sur D3 si nécessaire
   - intégrer le montage dans un petit boîtier
*/

#include <IRremote.hpp>

#define DEBUG 0 // 1=ON, 0=OFF

#define IR_PIN 2
#define OUT_PIN 3

// ------------------------------------------------------------
// Yamaha CDX-E410 NEC values
// ------------------------------------------------------------

#define YAMAHA_ADDR    0x78

#define CMD_STOP       0x01
#define CMD_PLAY       0x02
#define CMD_NEXT       0x03
#define CMD_PREV       0x04
#define CMD_RANDOM     0x07
#define CMD_TIME_INFO  0x0A
#define CMD_PROGRAM    0x0B
#define CMD_REPEAT     0x0C
#define CMD_POWER_KEY  0x0F

#define CMD_NUM_0      0x10
#define CMD_NUM_1      0x11
#define CMD_NUM_2      0x12
#define CMD_NUM_3      0x13
#define CMD_NUM_4      0x14
#define CMD_NUM_5      0x15
#define CMD_NUM_6      0x16
#define CMD_NUM_7      0x17
#define CMD_NUM_8      0x18
#define CMD_NUM_9      0x19

#define CMD_DISPLAY    0x4E
#define CMD_PAUSE      0xB9

#define CMD_POWER_ON   0x7E
#define CMD_STANDBY    0x7F
#define CMD_DIMMER_ON  0xFE
#define CMD_DIMMER_OFF 0xFF
#define CMD_VFD_OFF    0xFD

// ------------------------------------------------------------
// TVIX DVICO remote NEC values
// ------------------------------------------------------------
//
// Protocol : NEC
// Address  : 0xFD00
//
// Note:
// - flags = 0x00 : appui normal
// - flags = 0x01 : appui maintenu / repeat
//
// Mapping retenu:
// - POWER       -> Yamaha POWER ON / STANDBY
// - PLAY/PAUSE  -> Yamaha PLAY / PAUSE / RESUME
// - STOP        -> Yamaha STOP
// - NEXT        -> Yamaha NEXT track
// - RIGHT       -> Yamaha NEXT track
// - PREVIOUS    -> Yamaha PREVIOUS track
// - LEFT        -> Yamaha PREVIOUS track
// - INFO        -> Yamaha TIME / INFO
// - SHUFFLE     -> Yamaha RANDOM
// - GOTO        -> Yamaha REPEAT
// - SETUP       -> Yamaha PROGRAM
// - TV OUT      -> Yamaha DISPLAY / DIMMER / VFD OFF
// - 0..9        -> Yamaha direct numeric track selection
//

#define TVIX_ADDR          0xFD00

#define TVIX_SETUP         0x00  // USED -> Yamaha PROGRAM
#define TVIX_SHUFFLE       0x01  // USED -> Yamaha RANDOM
#define TVIX_MENU          0x02  // unused
#define TVIX_PICTURE       0x03  // unused
#define TVIX_VOL_PLUS      0x04  // unused, reserved for RX-E410 volume +
#define TVIX_VOL_MINUS     0x05  // unused, reserved for RX-E410 volume -
#define TVIX_ZOOM          0x06  // unused
#define TVIX_SUBTITLE      0x07  // unused
#define TVIX_OK            0x08  // unused
#define TVIX_LEFT          0x09  // USED -> Yamaha PREVIOUS track
#define TVIX_PREV          0x0A  // USED -> Yamaha PREVIOUS track
#define TVIX_DOWN          0x0B  // unused

#define TVIX_9             0x14  // USED -> Yamaha numeric 9
#define TVIX_GOTO          0x15  // USED -> Yamaha REPEAT
#define TVIX_PLAY_PAUSE    0x16  // USED -> Yamaha PLAY / PAUSE / RESUME
#define TVIX_0             0x17  // USED -> Yamaha numeric 0
#define TVIX_2             0x18  // USED -> Yamaha numeric 2
#define TVIX_1             0x19  // USED -> Yamaha numeric 1
#define TVIX_5             0x1A  // USED -> Yamaha numeric 5
#define TVIX_3             0x1B  // USED -> Yamaha numeric 3

#define TVIX_TITLE         0x40  // unused
#define TVIX_AUDIO         0x41  // unused
#define TVIX_ANGLE         0x42  // unused
#define TVIX_TV_OUT        0x43  // USED -> Yamaha DISPLAY / DIMMER / VFD OFF
#define TVIX_SCROLL        0x44  // unused
#define TVIX_MUTE          0x45  // unused, reserved for RX-E410 mute
#define TVIX_BOOKMARK      0x47  // unused
#define TVIX_RIGHT         0x49  // USED -> Yamaha NEXT track
#define TVIX_NEXT          0x4B  // USED -> Yamaha NEXT track

#define TVIX_UP            0x54  // unused
#define TVIX_INFO          0x55  // USED -> Yamaha TIME / INFO
#define TVIX_STOP          0x56  // USED -> Yamaha STOP
#define TVIX_POWER         0x57  // USED -> Yamaha POWER ON / STANDBY
#define TVIX_6             0x58  // USED -> Yamaha numeric 6
#define TVIX_7             0x59  // USED -> Yamaha numeric 7
#define TVIX_8             0x5A  // USED -> Yamaha numeric 8
#define TVIX_4             0x5B  // USED -> Yamaha numeric 4

// ------------------------------------------------------------
// Internal software states
// ------------------------------------------------------------
//
// CD_STANDBY : le lecteur est considéré comme en veille
// CD_STOPPED : le lecteur est considéré comme allumé et arrêté
// CD_PLAYING : le lecteur est considéré comme en lecture
// CD_PAUSED  : le lecteur est considéré comme en pause
//
// Attention:
// Ces états sont logiciels uniquement dans cette version.
// Si les boutons de façade du lecteur CD sont utilisés, l'état réel
// du lecteur peut différer de l'état suivi par l'Arduino.
//

enum CDState {
  CD_STANDBY,
  CD_STOPPED,
  CD_PLAYING,
  CD_PAUSED
};

// ------------------------------------------------------------
// Display software states
// ------------------------------------------------------------
//
// DISPLAY_BRIGHT : afficheur en luminosité normale
// DISPLAY_DIMMED : afficheur atténué
// DISPLAY_OFF    : afficheur éteint
//
// La touche mappée vers CMD_DISPLAY fait tourner ces états:
// BRIGHT -> DIMMED -> OFF -> BRIGHT
//

enum DisplayState {
  DISPLAY_BRIGHT,
  DISPLAY_DIMMED,
  DISPLAY_OFF
};

CDState cdState = CD_STOPPED;
DisplayState displayState = DISPLAY_BRIGHT;

// ------------------------------------------------------------
// Yamaha System Control NEC output
// ------------------------------------------------------------
//
// Le bus "System Control" du CDX-E410 attend un signal NEC démodulé.
// Dans ce montage:
// - lineLow() force la ligne active
// - lineRelease() relâche la ligne
//
// Le câblage utilisé inverse volontairement la logique de sortie:
// - digitalWrite HIGH pour tirer la ligne active
// - digitalWrite LOW pour relâcher la ligne
//

void lineLow() {
  digitalWrite(OUT_PIN, HIGH);
}

void lineRelease() {
  digitalWrite(OUT_PIN, LOW);
}

// ------------------------------------------------------------
// NEC bit sender
// ------------------------------------------------------------
//
// Chaque bit NEC est composé de:
// - LOW 560 us
// - HIGH 560 us pour un bit 0
// - HIGH 1690 us pour un bit 1
//

void sendBit(bool bit) {
  lineLow();
  delayMicroseconds(560);
  lineRelease();
  delayMicroseconds(bit ? 1690 : 560);
}

// ------------------------------------------------------------
// NEC byte sender
// ------------------------------------------------------------
//
// Le protocole NEC envoie les bits LSB first.
// On envoie donc le bit de poids faible, puis on décale vers la droite.
//

void sendByte(uint8_t data) {
  for (uint8_t i = 0; i < 8; i++) {
    sendBit(data & 0x01);
    data >>= 1;
  }
}

// ------------------------------------------------------------
// NEC frame sender
// ------------------------------------------------------------
//
// Trame envoyée:
// - header LOW 9000 us
// - header HIGH 4500 us
// - custom
// - ~custom
// - data / command
// - ~data / ~command
// - final LOW 560 us
//
// noInterrupts() est utilisé pour garder des timings NEC stables.
//

void sendNECFrame(uint8_t custom, uint8_t data) {
  noInterrupts();

  lineLow();
  delayMicroseconds(9000);
  lineRelease();
  delayMicroseconds(4500);

  sendByte(custom);
  sendByte((uint8_t)~custom);
  sendByte(data);
  sendByte((uint8_t)~data);

  lineLow();
  delayMicroseconds(560);
  lineRelease();

  interrupts();
}

// ------------------------------------------------------------
// Send Yamaha command
// ------------------------------------------------------------
//
// Envoie une commande NEC avec:
// - custom/address Yamaha CD = 0x78
// - command = commande Yamaha à transmettre au lecteur CD
//
// Le delay(150) sert d'anti-doublon simple et laisse le temps
// au lecteur de traiter la commande.
//

void sendYamaha(uint8_t cmd) {
#if DEBUG
  Serial.print(F("TX Yamaha -> addr=0x"));
  if (YAMAHA_ADDR < 0x10) Serial.print('0');
  Serial.print(YAMAHA_ADDR, HEX);
  Serial.print(F(" cmd=0x"));
  if (cmd < 0x10) Serial.print('0');
  Serial.println(cmd, HEX);
#endif

  sendNECFrame(YAMAHA_ADDR, cmd);

  // Petite temporisation anti-doublon / stabilité
  delay(150);
}

// ------------------------------------------------------------
// Debug helpers
// ------------------------------------------------------------

#if DEBUG

const char* protoToStr(decode_type_t proto) {
  switch (proto) {
    case NEC: return "NEC";
    case SONY: return "SONY";
    case RC5: return "RC5";
    case RC6: return "RC6";
    case PANASONIC: return "PANASONIC";
    case JVC: return "JVC";
    case SAMSUNG: return "SAMSUNG";
    case LG: return "LG";
    default: return "OTHER";
  }
}

void printHex8(uint8_t v) {
  if (v < 0x10) Serial.print('0');
  Serial.print(v, HEX);
}

void printHex16(uint16_t v) {
  if (v < 0x1000) Serial.print('0');
  if (v < 0x100)  Serial.print('0');
  if (v < 0x10)   Serial.print('0');
  Serial.print(v, HEX);
}

#endif

// ------------------------------------------------------------
// High-level CD behavior
// ------------------------------------------------------------
//
// Gestion du bouton POWER:
// - si l'état logiciel est CD_STANDBY:
//     envoie CMD_POWER_ON 0x7E
//     passe l'état logiciel à CD_STOPPED
// - sinon:
//     envoie CMD_STANDBY 0x7F
//     passe l'état logiciel à CD_STANDBY
//
// L'afficheur est remis logiciellement à DISPLAY_BRIGHT lors
// d'un changement d'état power.
//

void handlePowerToggle() {
  if (cdState == CD_STANDBY) {
    sendYamaha(CMD_POWER_ON);
    cdState = CD_STOPPED;
    displayState = DISPLAY_BRIGHT;
  } else {
    sendYamaha(CMD_STANDBY);
    cdState = CD_STANDBY;
    displayState = DISPLAY_BRIGHT;
  }
}

// ------------------------------------------------------------
// Handle mapped Yamaha command
// ------------------------------------------------------------
//
// Cette fonction reçoit toujours une commande Yamaha CD valide.
// Elle peut provenir:
// - directement de la télécommande Yamaha d'origine
// - du mapping TVIX -> Yamaha
//
// Gestion spéciale:
// - CMD_POWER_KEY est converti en POWER_ON ou STANDBY
// - CMD_PLAY devient PLAY / PAUSE / RESUME selon cdState
// - CMD_DISPLAY fait tourner les états d'affichage
// - toutes les autres commandes sont envoyées telles quelles
//

void handleCdCommand(uint8_t command) {
  if (command == CMD_POWER_KEY) {
    handlePowerToggle();
    return;
  }

  if (cdState == CD_STANDBY) {
#if DEBUG
    Serial.println(F("Ignore: CD en standby"));
#endif
    return;
  }

  switch (command) {
    case CMD_PLAY:
      if (cdState == CD_STOPPED) {
        sendYamaha(CMD_PLAY);
        cdState = CD_PLAYING;
      } else if (cdState == CD_PLAYING) {
        sendYamaha(CMD_PAUSE);
        cdState = CD_PAUSED;
      } else if (cdState == CD_PAUSED) {
        sendYamaha(CMD_PAUSE);
        cdState = CD_PLAYING;
      }
      break;

    case CMD_STOP:
      sendYamaha(CMD_STOP);
      cdState = CD_STOPPED;
      break;

    case CMD_NEXT:
      sendYamaha(CMD_NEXT);
      break;

    case CMD_PREV:
      sendYamaha(CMD_PREV);
      break;

    case CMD_DISPLAY:
      switch (displayState) {
        case DISPLAY_BRIGHT:
#if DEBUG
          Serial.println(F("DISPLAY: BRIGHT -> DIMMED"));
#endif
          sendYamaha(CMD_DIMMER_ON);
          displayState = DISPLAY_DIMMED;
          break;

        case DISPLAY_DIMMED:
#if DEBUG
          Serial.println(F("DISPLAY: DIMMED -> OFF"));
#endif
          sendYamaha(CMD_VFD_OFF);
          displayState = DISPLAY_OFF;
          break;

        case DISPLAY_OFF:
#if DEBUG
          Serial.println(F("DISPLAY: OFF -> BRIGHT"));
#endif
          sendYamaha(CMD_DIMMER_OFF);
          displayState = DISPLAY_BRIGHT;
          break;
      }
      break;

    default:
      sendYamaha(command);
      break;
  }
}

// ------------------------------------------------------------
// TVIX -> Yamaha mapping
// ------------------------------------------------------------
//
// Convertit une commande reçue depuis la télécommande TVIX DVICO
// vers une commande Yamaha CDX-E410.
//
// Retour:
// - true  : la touche TVIX est connue et mappée
// - false : la touche TVIX n'est pas utilisée
//
// Notes:
// - TVIX_NEXT et TVIX_RIGHT envoient tous les deux CMD_NEXT
// - TVIX_PREV et TVIX_LEFT envoient tous les deux CMD_PREV
// - les touches numériques TVIX sont remappées dans l'ordre Yamaha:
//     TVIX_0 -> CMD_NUM_0
//     TVIX_1 -> CMD_NUM_1
//     ...
//     TVIX_9 -> CMD_NUM_9
//

bool mapTvixToYamaha(uint8_t tvixCommand, uint8_t* yamahaCommand) {
  switch (tvixCommand) {
    case TVIX_POWER:
      *yamahaCommand = CMD_POWER_KEY;
      return true;

    case TVIX_PLAY_PAUSE:
      *yamahaCommand = CMD_PLAY;
      return true;

    case TVIX_STOP:
      *yamahaCommand = CMD_STOP;
      return true;

    case TVIX_NEXT:
    case TVIX_RIGHT:
      *yamahaCommand = CMD_NEXT;
      return true;

    case TVIX_PREV:
    case TVIX_LEFT:
      *yamahaCommand = CMD_PREV;
      return true;

    case TVIX_INFO:
      *yamahaCommand = CMD_TIME_INFO;
      return true;

    case TVIX_SHUFFLE:
      *yamahaCommand = CMD_RANDOM;
      return true;

    case TVIX_GOTO:
      *yamahaCommand = CMD_REPEAT;
      return true;

    case TVIX_SETUP:
      *yamahaCommand = CMD_PROGRAM;
      return true;

    case TVIX_TV_OUT:
      *yamahaCommand = CMD_DISPLAY;
      return true;

    case TVIX_0:
      *yamahaCommand = CMD_NUM_0;
      return true;

    case TVIX_1:
      *yamahaCommand = CMD_NUM_1;
      return true;

    case TVIX_2:
      *yamahaCommand = CMD_NUM_2;
      return true;

    case TVIX_3:
      *yamahaCommand = CMD_NUM_3;
      return true;

    case TVIX_4:
      *yamahaCommand = CMD_NUM_4;
      return true;

    case TVIX_5:
      *yamahaCommand = CMD_NUM_5;
      return true;

    case TVIX_6:
      *yamahaCommand = CMD_NUM_6;
      return true;

    case TVIX_7:
      *yamahaCommand = CMD_NUM_7;
      return true;

    case TVIX_8:
      *yamahaCommand = CMD_NUM_8;
      return true;

    case TVIX_9:
      *yamahaCommand = CMD_NUM_9;
      return true;

    default:
      return false;
  }
}

// ------------------------------------------------------------
// Arduino setup
// ------------------------------------------------------------
//
// Initialisation:
// - port série debug à 115200 bauds
// - OUT_PIN en sortie
// - ligne System Control relâchée
// - démarrage du récepteur IR sur IR_PIN
//

void setup() {
  pinMode(OUT_PIN, OUTPUT);
  lineRelease();

  IrReceiver.begin(IR_PIN, DISABLE_LED_FEEDBACK);

#if DEBUG
  Serial.begin(115200);
  Serial.println(F("Start"));
  Serial.println(F("DEBUG actif"));
  Serial.println(F("Yamaha remote addr = 0x0078"));
  Serial.println(F("TVIX remote addr   = 0xFD00"));
#endif
}

// ------------------------------------------------------------
// Main loop
// ------------------------------------------------------------
//
// Fonctionnement général:
// 1. Attend une trame IR.
// 2. Lit protocole, adresse, commande et flags.
// 3. Ignore tout ce qui n'est pas NEC.
// 4. Si adresse Yamaha:
//      - utilise directement la commande reçue.
// 5. Si adresse TVIX:
//      - mappe la commande TVIX vers une commande Yamaha.
// 6. Filtre les répétitions pour éviter les doubles actions.
// 7. Transmet la commande au gestionnaire CD.
//
// Important:
// IrReceiver.resume() est appelé après lecture pour permettre
// la réception de la prochaine trame IR.
//

void loop() {
  if (!IrReceiver.decode()) {
    return;
  }

  decode_type_t protocol = IrReceiver.decodedIRData.protocol;
  uint16_t address = IrReceiver.decodedIRData.address;
  uint8_t command = IrReceiver.decodedIRData.command;
  uint8_t flags = IrReceiver.decodedIRData.flags;

#if DEBUG
  Serial.print(F("RX proto="));
  Serial.print(protoToStr(protocol));
  Serial.print(F(" address=0x"));
  printHex16(address);
  Serial.print(F(" command=0x"));
  printHex8(command);
  Serial.print(F(" flags=0x"));
  printHex8(flags);
  if (flags & IRDATA_FLAGS_IS_REPEAT) {
    Serial.print(F(" REPEAT"));
  }
  Serial.println();
#endif

  IrReceiver.resume();

  if (protocol != NEC) {
#if DEBUG
    Serial.println(F("Ignore: protocole non NEC"));
#endif
    return;
  }

  bool isRepeat = flags & IRDATA_FLAGS_IS_REPEAT;
  uint8_t yamahaCommand = 0x00;

  // ----------------------------------------------------------
  // Télécommande Yamaha d'origine
  // ----------------------------------------------------------
  //
  // Les commandes Yamaha sont déjà au bon format.
  // On les transmet donc directement, sauf filtrage repeat.
  //

  if (address == YAMAHA_ADDR) {
    yamahaCommand = command;

    // On ignore les répétitions pour éviter les doubles commandes,
    // sauf NEXT / PREV pour permettre le maintien éventuel.
    if (isRepeat && command != CMD_NEXT && command != CMD_PREV) {
#if DEBUG
      Serial.println(F("Ignore: repetition Yamaha"));
#endif
      return;
    }
  }

  // ----------------------------------------------------------
  // Télécommande TVIX DVICO complémentaire
  // ----------------------------------------------------------
  //
  // Les commandes TVIX n'ont pas les mêmes codes que Yamaha.
  // On utilise donc mapTvixToYamaha() pour convertir la touche
  // TVIX reçue vers une commande Yamaha CDX-E410.
  //

  else if (address == TVIX_ADDR) {
    if (!mapTvixToYamaha(command, &yamahaCommand)) {
#if DEBUG
      Serial.println(F("Ignore: touche TVIX non mappee"));
#endif
      return;
    }

    // Sur la TVIX, un appui maintenu passe flags à 0x01.
    //
    // On ignore les répétitions sauf:
    // - TVIX_NEXT
    // - TVIX_PREV
    // - TVIX_RIGHT
    // - TVIX_LEFT
    //
    // Les touches numériques 0..9 sont donc envoyées une seule fois,
    // même si l'appui est un peu trop long.
    if (isRepeat &&
        command != TVIX_NEXT &&
        command != TVIX_PREV &&
        command != TVIX_RIGHT &&
        command != TVIX_LEFT) {
#if DEBUG
      Serial.println(F("Ignore: repetition TVIX"));
#endif
      return;
    }

#if DEBUG
    Serial.print(F("TVIX mapped -> Yamaha cmd=0x"));
    printHex8(yamahaCommand);
    Serial.println();
#endif
  }

  // ----------------------------------------------------------
  // Autre télécommande / adresse inconnue
  // ----------------------------------------------------------

  else {
#if DEBUG
    Serial.println(F("Ignore: adresse inconnue"));
#endif
    return;
  }

  // Petite pause avant traitement effectif.
  // Utile pour stabiliser le comportement et limiter les doublons.
  delay(30);

  handleCdCommand(yamahaCommand);
}