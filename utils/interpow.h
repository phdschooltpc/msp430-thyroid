/*
 *
 * InterPow: library for operation under intermittent power.
 *
 * TODO:
 * - [M] Return errors for Read/Write operations in case of reading/writing
 *       a field from another channel or any other invalid option
 *
 * Authors: Carlo Delle Donne, Dimitris Patoukas, Thijmen Ketel
 *
 * Credits: developed based on the Chain idea (paper by A. Colin and B. Lucia),
 *          available at https://brandonlucia.com/pubs/chain.pdf.
 *
 */

#ifndef INC_INTERPOW_H_
#define INC_INTERPOW_H_

/**
 *
 * \addtogroup interpow_api Intermittent Power
 * @{
 *
 * Functions for usage of a transiently-powered device, including Task and Field
 * declaration, program flow management and read/write operations.
 *
 * \author  Carlo Delle Donne
 * \author  Dimitrios Patoukas
 * \author  Thijmen Ketel
 *
 * \defgroup interpow_declaration Preliminary declarations
 * Macros for preliminary Task and Field declarations.
 *
 * \par Channels
 * Channels allow two tasks to communicate with each other, or a task to
 * communicate with itself. A channel is defined by its \e source and
 * \e destination tasks, and is referred to as (\e source, \e destination).
 *
 * \par Declarations
 * Declarations of tasks, initial task, fields and self-fields must be in the
 * global area outside the \e main, and have to follow this order:
 *   \li tasks declarations, using `NewTask()`;
 *   \li initial task declaration, using `InitialTask()`;
 *   \li fields and self-fields declarations, using `NewField()` and
 *       `NewSelfField()`.
 *
 * \defgroup interpow_flow Program flow
 * Macros for program flow management, i.e. resume program and switch task.
 *
 * \defgroup interpow_read_write Read and Write
 * Macros for read/write operations, differentiated by field types.
 *
 * @}
 */

#include <stdint.h>


/*
 *******************************************************************************
 *******************************************************************************
 *
 *  #define groups, use them!
 *
 *******************************************************************************
 *******************************************************************************
 */

/**
 * Self-field codes for self-field declarations
 *
 * \defgroup SELF_FIELD_CODES
 * @{
 */
#define SELF_FIELD_CODE_1           0x01
#define SELF_FIELD_CODE_2           0x02
#define SELF_FIELD_CODE_3           0x04
#define SELF_FIELD_CODE_4           0x08
#define SELF_FIELD_CODE_5           0x10
#define SELF_FIELD_CODE_6           0x20
#define SELF_FIELD_CODE_7           0x40
#define SELF_FIELD_CODE_8           0x80
/**
 * @}
 */

/**
 * Types for field/self-field declarations
 *
 * \defgroup FIELD_TYPES
 * @{
 */
#define INT8                        int8_t
#define UINT8                       uint8_t
#define INT16                       int16_t
#define UINT16                      uint16_t
#define INT32                       int32_t
#define UINT32                      uint32_t
#define FLOAT32                     float
/**
 * @}
 */


/*
 *******************************************************************************
 *******************************************************************************
 *
 *  Basic structures definition, only for internal use!
 *
 *******************************************************************************
 *******************************************************************************
 */

typedef struct __field {
    void            *base_addr;
    uint16_t        length;
} __field;

typedef struct __self_field {
    uint8_t         code;
    void            *base_addr_0;
    void            *base_addr_1;
    uint16_t        length;
} __self_field;

typedef struct __task {
    void            (*task_function) (void);
    uint8_t         has_self_channel;
    uint16_t        sf_state;
} __task;

typedef struct __program_state {
    __task          *curr_task;
} __program_state;


/*
 *******************************************************************************
 *******************************************************************************
 *
 *  Function prototypes, only for internal use!
 *
 *******************************************************************************
 *******************************************************************************
 */

void read_field_8(void*, int8_t*, uint8_t, __program_state*);
void read_field_u8(void*, uint8_t*, uint8_t, __program_state*);
void read_field_16(void*, int16_t*, uint8_t, __program_state*);
void read_field_u16(void*, uint16_t*, uint8_t, __program_state*);
void read_field_32(void*, int32_t*, uint8_t, __program_state*);
void read_field_u32(void*, uint32_t*, uint8_t, __program_state*);
void read_field_f32(void*, float*, uint8_t, __program_state*);

void write_field_8(void*, int8_t*, uint8_t, __program_state*);
void write_field_u8(void*, uint8_t*, uint8_t, __program_state*);
void write_field_16(void*, int16_t*, uint8_t, __program_state*);
void write_field_u16(void*, uint16_t*, uint8_t, __program_state*);
void write_field_32(void*, int32_t*, uint8_t, __program_state*);
void write_field_u32(void*, uint32_t*, uint8_t, __program_state*);
void write_field_f32(void*, float*, uint8_t, __program_state*);

void write_field_element_8(__field*, int8_t*, uint16_t);
void write_field_element_u8(__field*, uint8_t*, uint16_t);
void write_field_element_16(__field*, int16_t*, uint16_t);
void write_field_element_u16(__field*, uint16_t*, uint16_t);
void write_field_element_32(__field*, int32_t*, uint16_t);
void write_field_element_u32(__field*, uint32_t*, uint16_t);
void write_field_element_f32(__field*, float*, uint16_t);

void start_task(__task*, __program_state*);

void resume_program(__program_state*);


/*
 *******************************************************************************
 *******************************************************************************
 *
 *  Core macro "functions", use them!
 *
 *******************************************************************************
 *******************************************************************************
 */

#define PersState                   __prog_state

#define PersField(S, D, F)          __##S##D##F

#define PersSField0(T, F)           __##T##T##F##_0
#define PersSField1(T, F)           __##T##T##F##_1

#define GetField(S, D, F)           __##S##D##F##__


/**
 * \ingroup interpow_declaration
 *
 * \hideinitializer
 *
 * \brief Create a new task.
 *
 * @param NAME              task's name
 * @param FN                pointer to the function to execute
 * @param HAS_SELF_CHANNEL  1 if task has self-channel, 0 otherwise
 *
 * \par Non-volatility
 * Tasks have to be non-volatile, and have to be initialised only at the first
 * start of the program. In order to do so, before declaring a task \e myTask,
 * a `#pragma` has to be used, i.e.
 * \verbatim
   #pragma PERSISTENT(myTask)
   NewTask(myTask, myFunction, x)
   \endverbatim
 * where x = 0, 1.
 */
#define NewTask(NAME, FN, HAS_SELF_CHANNEL)                                     \
        __task NAME = {                                                         \
            .task_function = FN,                                                \
            .has_self_channel = HAS_SELF_CHANNEL,                               \
            .sf_state = 0                                                       \
        };


/**
 * \ingroup interpow_declaration
 *
 * \hideinitializer
 *
 * \brief Define which task has to execute first on the very first start of 
 *        the system. \e TASK has to be defined before using this macro.
 *
 * @param TASK  name of the task
 *
 * \par Non-volatility
 * The library requires the user to use a `#pragma` before calling this macro,
 * in order to save the program state to non-volatile memory, i.e.
 * \verbatim
   #pragma PERSISTENT(PersState)
   InitialTask(myInitialTask)
   \endverbatim
 * Note that the argument of `PERSISTENT` is fixed.
 */
#define InitialTask(TASK)                                                       \
        static __program_state __prog_state = {                                 \
            .curr_task = &TASK,                                                 \
        };


/**
 * \ingroup interpow_declaration
 *
 * \hideinitializer
 *
 * \brief Define a new field, conceptually belonging to the channel (\e SRC, 
 *        \e DST ). \e SRC and \e DST have to be defined before using this macro.
 *
 * @param SRC   channel's source task
 * @param DST   channel's destination task
 * @param NAME  field's name
 * @param TYPE  field's TYPE, must be a value of \ref FIELD_TYPES
 * @param LEN   field's length (if LEN>1 the field is an array)
 *
 * \par Non-volatility
 * Fields have to be non-volatile, and have to be initialised only at the first
 * start of the program. In order to do so, before declaring a field \e myField,
 * belonging to the channel (\e ST, \e DT ), a `#pragma` has to be used, i.e.
 * \verbatim
   #pragma PERSISTENT(PersField(ST, DT, myField))
   NewField(ST, DT, myField, type, len)
   \endverbatim
 */
#define NewField(SRC, DST, NAME, TYPE, LEN)                                     \
        TYPE __##SRC##DST##NAME[LEN] = {0};                                     \
        __field __##SRC##DST##NAME##__ = {                                      \
            .length = LEN,                                                      \
            .base_addr = &__##SRC##DST##NAME                                    \
        };


/**
 * \ingroup interpow_declaration
 *
 * \hideinitializer
 *
 * \brief Define a new self-field, conceptually belonging to the self-channel
 *        (\e TASK, \e TASK ). \e TASK has to be defined before using this macro.
 *
 * @param TASK  channel's source and destination task
 * @param NAME  field's name
 * @param TYPE  field's TYPE, must be a value of \ref FIELD_TYPES
 * @param LEN   field's length (if LEN>1 the field is an array)
 * @param CODE  field's code, must be a value of \ref SELF_FIELD_CODES
 *
 * \par Self-field code
 * A specific self-channel (\e T, \e T ) can have at most 8 self-fields.
 * Self-fields belonging to the same self-channel <b>must have different 
 * codes</b>, but they do not need to be sequential.
 *
 * \par Non-volatility
 * Self-fields have to be non-volatile, and have to be initialised only at the first
 * start of the program. In order to do so, before declaring a self-field \e myField,
 * belonging to the self-channel (\e T, \e T ), two `#pragma` have to be used, i.e.
 * \verbatim
   #pragma PERSISTENT(PersSField0(T, myField))
   #pragma PERSISTENT(PersSField1(T, myField))
   NewSelfField(T, myField, type, len, code)
   \endverbatim
 */
#define NewSelfField(TASK, NAME, TYPE, LEN, CODE)                               \
        TYPE __##TASK##TASK##NAME##_0[LEN] = {0};                               \
        TYPE __##TASK##TASK##NAME##_1[LEN] = {0};                               \
        __self_field __##TASK##TASK##NAME##__ = {                               \
            .length = LEN,                                                      \
            .base_addr_0 = &__##TASK##TASK##NAME##_0,                           \
            .base_addr_1 = &__##TASK##TASK##NAME##_1,                           \
            .code = CODE                                                        \
        };


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Read 8-bit signed integer field from channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to read
 * @param DST       address to store the result at
 */
#define ReadField_8(SRC_TASK, DST_TASK, FLD, DST)                               \
        read_field_8(&GetField(SRC_TASK, DST_TASK, FLD), DST, 0, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Read 8-bit unsigned integer field from channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to read
 * @param DST       address to store the result at
 */
#define ReadField_U8(SRC_TASK, DST_TASK, FLD, DST)                              \
        read_field_u8(&GetField(SRC_TASK, DST_TASK, FLD), DST, 0, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Read 8-bit signed integer self-field from channel (\e TASK, \e TASK ).
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to read
 * @param DST   address to store the result at
 */
#define ReadSelfField_8(TASK, FLD, DST)                                         \
        read_field_8(&GetField(TASK, TASK, FLD), DST, 1, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Read 8-bit unsigned integer self-field from channel (\e TASK, \e TASK ).
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to read
 * @param DST   address to store the result at
 */
#define ReadSelfField_U8(TASK, FLD, DST)                                        \
        read_field_u8(&GetField(TASK, TASK, FLD), DST, 1, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Read 16-bit signed integer field from channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to read
 * @param DST       address to store the result at
 */
#define ReadField_16(SRC_TASK, DST_TASK, FLD, DST)                              \
        read_field_16(&GetField(SRC_TASK, DST_TASK, FLD), DST, 0, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Read 16-bit unsigned integer field from channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to read
 * @param DST       address to store the result at
 */
#define ReadField_U16(SRC_TASK, DST_TASK, FLD, DST)                             \
        read_field_u16(&GetField(SRC_TASK, DST_TASK, FLD), DST, 0, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Read 16-bit signed integer self-field from channel (\e TASK, \e TASK ).
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to read
 * @param DST   address to store the result at
 */
#define ReadSelfField_16(TASK, FLD, DST)                                        \
        read_field_16(&GetField(TASK, TASK, FLD), DST, 1, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Read 16-bit unsigned integer self-field from channel (\e TASK, \e TASK ).
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to read
 * @param DST   address to store the result at
 */
#define ReadSelfField_U16(TASK, FLD, DST)                                       \
        read_field_u16(&GetField(TASK, TASK, FLD), DST, 1, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Read 32-bit signed integer field from channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to read
 * @param DST       address to store the result at
 */
#define ReadField_32(SRC_TASK, DST_TASK, FLD, DST)                              \
        read_field_32(&GetField(SRC_TASK, DST_TASK, FLD), DST, 0, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Read 32-bit unsigned integer field from channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to read
 * @param DST       address to store the result at
 */
#define ReadField_U32(SRC_TASK, DST_TASK, FLD, DST)                             \
        read_field_u32(&GetField(SRC_TASK, DST_TASK, FLD), DST, 0, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Read 32-bit signed integer self-field from channel (\e TASK, \e TASK ).
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to read
 * @param DST   address to store the result at
 */
#define ReadSelfField_32(TASK, FLD, DST)                                        \
        read_field_32(&GetField(TASK, TASK, FLD), DST, 1, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Read 32-bit unsigned integer self-field from channel (\e TASK, \e TASK ).
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to read
 * @param DST   address to store the result at
 */
#define ReadSelfField_U32(TASK, FLD, DST)                                       \
        read_field_u32(&GetField(TASK, TASK, FLD), DST, 1, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Read 32-bit float field from channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to read
 * @param DST       address to store the result at
 */
#define ReadField_F32(SRC_TASK, DST_TASK, FLD, DST)                               \
        read_field_f32(&GetField(SRC_TASK, DST_TASK, FLD), DST, 0, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Read 32-bit float self-field from channel (\e TASK, \e TASK ).
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to read
 * @param DST   address to store the result at
 */
#define ReadSelfField_F32(TASK, FLD, DST)                                         \
        read_field_f32(&GetField(TASK, TASK, FLD), DST, 1, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write 8-bit signed integer field in channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 */
#define WriteField_8(SRC_TASK, DST_TASK, FLD, SRC)                              \
        write_field_8(&GetField(SRC_TASK, DST_TASK, FLD), SRC, 0, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write 8-bit unsigned integer field in channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 */
#define WriteField_U8(SRC_TASK, DST_TASK, FLD, SRC)                             \
        write_field_u8(&GetField(SRC_TASK, DST_TASK, FLD), SRC, 0, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write 8-bit signed integer self-field in channel (\e TASK, \e TASK ).
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to write
 * @param SRC   address of the variable to write into the field
 */
#define WriteSelfField_8(TASK, FLD, SRC)                                        \
        write_field_8(&GetField(TASK, TASK, FLD), SRC, 1, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write 8-bit unsigned integer self-field in channel (\e TASK, \e TASK ).
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to write
 * @param SRC   address of the variable to write into the field
 */
#define WriteSelfField_U8(TASK, FLD, SRC)                                       \
        write_field_u8(&GetField(TASK, TASK, FLD), SRC, 1, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write 16-bit signed integer field in channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 */
#define WriteField_16(SRC_TASK, DST_TASK, FLD, SRC)                             \
        write_field_16(&GetField(SRC_TASK, DST_TASK, FLD), SRC, 0, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write 16-bit unsigned integer field in channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 */
#define WriteField_U16(SRC_TASK, DST_TASK, FLD, SRC)                            \
        write_field_u16(&GetField(SRC_TASK, DST_TASK, FLD), SRC, 0, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write 16-bit signed integer self-field in channel (\e TASK, \e TASK ).
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to write
 * @param SRC   address of the variable to write into the field
 */
#define WriteSelfField_16(TASK, FLD, SRC)                                       \
        write_field_16(&GetField(TASK, TASK, FLD), SRC, 1, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write 16-bit unsigned integer self-field in channel (\e TASK, \e TASK ).
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to write
 * @param SRC   address of the variable to write into the field
 */
#define WriteSelfField_U16(TASK, FLD, SRC)                                      \
        write_field_u16(&GetField(TASK, TASK, FLD), SRC, 1, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write 32-bit signed integer field in channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 */
#define WriteField_32(SRC_TASK, DST_TASK, FLD, SRC)                             \
        write_field_32(&GetField(SRC_TASK, DST_TASK, FLD), SRC, 0, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write 32-bit unsigned integer field in channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 */
#define WriteField_U32(SRC_TASK, DST_TASK, FLD, SRC)                            \
        write_field_u32(&GetField(SRC_TASK, DST_TASK, FLD), SRC, 0, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write 32-bit signed integer self-field in channel (\e TASK, \e TASK ).
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to write
 * @param SRC   address of the variable to write into the field
 */
#define WriteSelfField_32(TASK, FLD, SRC)                                       \
        write_field_32(&GetField(TASK, TASK, FLD), SRC, 1, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write 32-bit unsigned integer self-field in channel (\e TASK, \e TASK ).
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to write
 * @param SRC   address of the variable to write into the field
 */
#define WriteSelfField_U32(TASK, FLD, SRC)                                      \
        write_field_u32(&GetField(TASK, TASK, FLD), SRC, 1, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write 32-bit float field in channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 */
#define WriteField_F32(SRC_TASK, DST_TASK, FLD, SRC)                              \
        write_field_f32(&GetField(SRC_TASK, DST_TASK, FLD), SRC, 0, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write 32-bit float self-field in channel (\e TASK, \e TASK ).
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to write
 * @param SRC   address of the variable to write into the field
 */
#define WriteSelfField_F32(TASK, FLD, SRC)                                        \
        write_field_f32(&GetField(TASK, TASK, FLD), SRC, 1, &__prog_state);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write a single element of a 8-bit signed field in channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 * @param POS       offset of the element to write
 */
#define WriteFieldElement_8(SRC_TASK, DST_TASK, FLD, SRC, POS)                  \
        write_field_element_8(&GetField(SRC_TASK, DST_TASK, FLD), SRC, POS);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write a single element of a 8-bit unsigned field in channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 * @param POS       offset of the element to write
 */
#define WriteFieldElement_U8(SRC_TASK, DST_TASK, FLD, SRC, POS)                 \
        write_field_element_u8(&GetField(SRC_TASK, DST_TASK, FLD), SRC, POS);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write a single element of a 16-bit signed field in channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 * @param POS       offset of the element to write
 */
#define WriteFieldElement_16(SRC_TASK, DST_TASK, FLD, SRC, POS)                 \
        write_field_element_16(&GetField(SRC_TASK, DST_TASK, FLD), SRC, POS);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write a single element of a 16-bit unsigned field in channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 * @param POS       offset of the element to write
 */
#define WriteFieldElement_U16(SRC_TASK, DST_TASK, FLD, SRC, POS)                \
        write_field_element_u16(&GetField(SRC_TASK, DST_TASK, FLD), SRC, POS);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write a single element of a 32-bit signed field in channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 * @param POS       offset of the element to write
 */
#define WriteFieldElement_32(SRC_TASK, DST_TASK, FLD, SRC, POS)                 \
        write_field_element_32(&GetField(SRC_TASK, DST_TASK, FLD), SRC, POS);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write a single element of a 32-bit unsigned field in channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 * @param POS       offset of the element to write
 */
#define WriteFieldElement_U32(SRC_TASK, DST_TASK, FLD, SRC, POS)                \
        write_field_element_u32(&GetField(SRC_TASK, DST_TASK, FLD), SRC, POS);


/**
 * \ingroup interpow_read_write
 *
 * \hideinitializer
 *
 * \brief
 * Write a single element of a 32-bit float field in channel (\e SRC_TASK, \e DST_TASK ).
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 * @param POS       offset of the element to write
 */
#define WriteFieldElement_F32(SRC_TASK, DST_TASK, FLD, SRC, POS)                  \
        write_field_element_f32(&GetField(SRC_TASK, DST_TASK, FLD), SRC, POS);


/**
 * \ingroup interpow_flow
 *
 * \hideinitializer
 *
 * \brief
 * Switch to another task.
 *
 * @param TASK  task to switch to
 */
#define StartTask(TASK)                                                         \
        start_task(&TASK, &__prog_state);


/**
 * \ingroup interpow_flow
 *
 * \hideinitializer
 *
 * \brief
 * Resume program from last executing task. Call inside the \e main's loop.
 */
#define Resume()                                                                \
        resume_program(&__prog_state);


#endif /* INC_INTERPOW_H_ */
