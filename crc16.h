#ifndef CRC16_H
#define CRC16_H


//---------------------------------------------------------------------------
/// Функция расчета 16-ти битной контрольной суммы.
/// \param buf - Буфер по которому считаем контрольную сумму.
/// \param len - Размер буфера.
/// \return Возвращает 16-ти битную контрольную сумму.
extern unsigned short CRC16(unsigned short *buf, unsigned short len);
//extern unsigned short __fastcall CRC16a(unsigned short *buf, unsigned short len);
unsigned short CRC16modbus(unsigned char *buf, unsigned short len);
//---------------------------------------------------------------------------

#endif // CRC16_H
