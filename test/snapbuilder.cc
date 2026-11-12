#include <ddnet_protocol/chunk.h>
#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/message.h>
#include <ddnet_protocol/msg_system.h>
#include <ddnet_protocol/packer.h>
#include <ddnet_protocol/packet.h>
#include <ddnet_protocol/session.h>
#include <ddnet_protocol/snap_builder.h>
#include <ddnet_protocol/snap_items.h>
#include <ddnet_protocol/snapshot.h>

#include <gtest/gtest.h>

TEST(SnapBuilder, AddExplosion) {
	DDProtoSnapBuilder builder = {};
	DDProtoError err = DDPROTO_ERR_NONE;
	ddproto_snap_builder_init(&builder);
	DDProtoEventExplosion *explosion = (DDProtoEventExplosion *)ddproto_snap_builder_new_item(&builder, DDPROTO_ITEM_EXPLOSION, 0, sizeof(DDProtoEventExplosion), &err);
	explosion->x = 10;
	explosion->y = 10;
	EXPECT_EQ(err, DDPROTO_ERR_NONE);
	EXPECT_EQ(builder.num_items, 1);
	DDProtoSnapshot snapshot = {};
	ddproto_snap_builder_finish(&builder, &snapshot, &err);
	EXPECT_EQ(err, DDPROTO_ERR_NONE);
	EXPECT_EQ(snapshot.items.len, 1);
	ddproto_free_snapshot(&snapshot);
}
