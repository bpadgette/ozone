#ifndef OZONE_GENERATOR_H
#define OZONE_GENERATOR_H

typedef enum OzoneGeneratorStateEnum {
  OZONE_GENERATOR_PENDING,
  OZONE_GENERATOR_REJECTED,
  OZONE_GENERATOR_RESOLVED,
} OzoneGeneratorState;

#define OZONE_GENERATOR_DECLARE_API(_type_)                                                                            \
  typedef struct _type_##GeneratorStruct {                                                                             \
    OzoneGeneratorState state;                                                                                         \
    int re_entry_label;                                                                                                \
    _type_ context;                                                                                                    \
  } _type_##Generator;

#define ozoneGenerator(_type_, _context_)                                                                              \
  (_type_##Generator) { .context = (_context_) }

#define ozoneGeneratorPending(_generator_) ((_generator_)->state == OZONE_GENERATOR_PENDING)

#define ozoneGeneratorBegin(_generator_)                                                                               \
  if (!ozoneGeneratorPending(_generator_))                                                                             \
    return;                                                                                                            \
  switch ((_generator_)->re_entry_label) {                                                                             \
  case 0:                                                                                                              \
    do {                                                                                                               \
    } while (0)

#define ozoneGeneratorYield(_generator_)                                                                               \
  (_generator_)->re_entry_label = __LINE__;                                                                            \
  return;                                                                                                              \
  case __LINE__:                                                                                                       \
    do {                                                                                                               \
    } while (0)

#define ozoneGeneratorReject(_generator_)                                                                              \
  do {                                                                                                                 \
    (_generator_)->state = OZONE_GENERATOR_REJECTED;                                                                   \
    return;                                                                                                            \
  } while (0)

#define ozoneGeneratorResolve(_generator_)                                                                             \
  (_generator_)->state = OZONE_GENERATOR_RESOLVED;                                                                     \
  (_generator_)->re_entry_label = __LINE__;                                                                            \
  case __LINE__:                                                                                                       \
  default:                                                                                                             \
    return;                                                                                                            \
    }                                                                                                                  \
    do {                                                                                                               \
    } while (0)

#endif
