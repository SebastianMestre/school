#include "serialization.h"

#include "io.h"
#include "string.h"
#include "contact.h"
#include "storage.h"
#include "vector.h"

#include <stdio.h>

void
write_contact(Contact* contact, bool braces, FILE* f) {
	if (braces) fprintf(f, "{");
	print_title_case(contact->name, f);
	fprintf(f, ",");
	print_title_case(contact->surname, f);
	fprintf(f, ",%u,%s", contact->age, contact->phone_number);
	if (braces) fprintf(f, "}");
}

void
write_vector_of_contacts(Vector const* contacts, bool braces, FILE* f) {
	for (size_t i = 0; i < contacts->size; ++i) {
		Contact* contact; span_write(&contact, vector_at(contacts, i));
		write_contact(contact, braces, f);
		fprintf(f, "\n");
	}
}

void
write_vector_of_contacts_by_id(Storage* storage, Vector const* contacts, bool braces, FILE* f) {
	for (size_t i = 0; i < contacts->size; ++i) {
		ContactId id; span_write(&id, vector_at(contacts, i));
		Contact* contact = storage_at(storage, id);
		write_contact(contact, braces, f);
		fprintf(f, "\n");
	}
}

ReadContactStatus
read_contact(char* buf, size_t n, IncompleteContact* out, FILE* f) {
	bool success = get_line(buf, n, f);

	if (!success)
		return RCS_E_BAD_LINE;

	return deserialize_contact(buf, out);
}

ReadContactStatus
deserialize_contact(char* buf, IncompleteContact* out) {

		out->name = nullptr;
		out->surname = nullptr;
		out->has_age = false;
		out->age = 0;
		out->phone_number = nullptr;

		out->name = buf;

		for (; *buf != ','; ++buf)
			if (*buf == '\0')
				return RCS_E_FIELD_COUNT;

		*buf++ = '\0'; // name terminator

		out->surname = buf;

		for (; *buf != ','; ++buf)
			if (*buf == '\0')
				return RCS_E_FIELD_COUNT;

		*buf++ = '\0'; // surname terminator

		char* age_str = buf;

		for (; *buf != ','; ++buf)
			if (*buf == '\0')
				return RCS_E_FIELD_COUNT;

		*buf++ = '\0'; // age terminator

		out->phone_number = buf;

		for (; *buf != '\0'; ++buf)
			if (*buf == ',')
				return RCS_E_FIELD_COUNT;

		string_trim(out->name);
		string_trim(out->surname);
		string_trim(age_str);
		string_trim(out->phone_number);

		uint32_t age;
		bool success = parse_u32(age_str, &age);
		if (!success)
			return RCS_E_INVALID_AGE;

		out->age = age;
		out->has_age = true;

		return RCS_OK;
}
