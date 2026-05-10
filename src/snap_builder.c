#include <ddnet_protocol/snapshot.h>

#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/int_string.h>
#include <ddnet_protocol/packer.h>
#include <ddnet_protocol/snap_builder.h>

void ddproto_snap_builder_init(DDProtoSnapBuilder *builder) {
	builder->num_items = 0;
	builder->data_size = 0;
}

void *ddproto_snap_builder_new_item(DDProtoSnapBuilder *builder, int32_t item_type, int32_t item_id, int32_t item_size, DDProtoError *err) {
	int32_t *item = (int32_t *)(builder->data + builder->data_size);
	item[0] = (item_type << 16) | (item_id & 0xffff);
	builder->offsets[builder->num_items++] = builder->data_size;
	builder->data_size += (int32_t)sizeof(int32_t) + item_size;
	return item + 1;
}

int32_t ddproto_snap_builder_finish(DDProtoSnapBuilder *builder, DDProtoSnapshot *snap, DDProtoError *err) {
	snap->items.len = builder->num_items;
	snap->items.data = malloc(sizeof(DDProtoSnapItem) * snap->items.len);

	// TODO: reference implementation sorts items here

	for(int32_t i = 0; i < builder->num_items; i++) {
		// memcpy(snap->items.data[i], builder->num_items
		// TODO: copy data but idk
	}

	return 0;
}

// bro idk

// void ddproto_snap_add_item(DDProtoSnapshot *snap, DDProtoSnapItem *item) {
// 	snap->items.data[snap->items.len++] = *item;
// }

// void foo(void) {
// 	DDProtoSnapshot snap = {};
// 	snap.items.data[0].item.flag
// }

// // copy sorted items
//
//
// int OffsetCur = 0;
// for(int i = 0; i < NumItems; i++)
// {
// 	pSnap->Offsets()[i] = OffsetCur;
// 	mem_copy(pSnap->DataStart()+OffsetCur, m_aData + m_aOffsets[i], aItemSizes[i]);
// 	OffsetCur += aItemSizes[i];
// }
