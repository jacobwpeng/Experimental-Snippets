/*
 * =====================================================================================
 *
 *       Filename:  rbtree_unittest.cc
 *        Created:  08/24/14 13:18:08
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "rbtree.h"
#include <ctime>
#include <cstdlib>
#include <map>
#include <gtest/gtest.h>
#include <boost/scoped_ptr.hpp>

using fx::base::container::RBTree;

class RBTreeUnittest : public ::testing::Test
{
    protected:
        virtual void SetUp()
        {
            ::srand(::time(NULL));
        }

        virtual void TearDown()
        {

        }
};

TEST_F(RBTreeUnittest, Create)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    boost::scoped_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    EXPECT_TRUE (rb != NULL);
    EXPECT_EQ (0, rb->size());
    //RBTree header = 32, MemoryList header = 48Bytes, RBNode size = 24Bytes, Slice pointer = 8Bytes
    //capacity = floor((65536 - 32 - 48) / (24 + 8)) == 2045;
    EXPECT_EQ (2045, rb->capacity());
}

TEST_F(RBTreeUnittest, Put)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    boost::scoped_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (rb != NULL);

    unsigned kMaxRandomNumber = 1000000;
    rb->Put(kMaxRandomNumber + 1, kMaxRandomNumber + 2);
    EXPECT_EQ (rb->size(), 1);
    for (unsigned i = 2; ; ++i)
    {
        unsigned key, val;
        key = val = rand() % 1000000;
        bool ok = rb->Put(key, val);
        if (not ok) break;
    }

    EXPECT_EQ (rb->size(), rb->capacity());
    EXPECT_LE (rb->depth(), 10);

    bool ok = rb->Put(kMaxRandomNumber + 1, kMaxRandomNumber + 3);
    EXPECT_TRUE (ok);                           /* overwrite exists value */
    ok = rb->Put(kMaxRandomNumber + 2, kMaxRandomNumber);
    EXPECT_TRUE (not ok);                       /* try to insert new key when no space left */
}

TEST_F(RBTreeUnittest, Get)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    boost::scoped_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (rb != NULL);

    MapType::iterator rb_iter = rb->Get(0);
    EXPECT_EQ (rb_iter, rb->end());

    std::map<unsigned, unsigned> m;
    while (1)
    {
        unsigned key, val;
        key = rand();
        val = rand();
        if (not rb->Put(key, val)) break;
        m[key] = val;
    }

    EXPECT_EQ (m.size(), rb->size());
    EXPECT_LE (rb->depth(), 10);
    std::map<unsigned, unsigned>::const_iterator diff = m.end();
    for (std::map<unsigned, unsigned>::const_iterator iter = m.begin();
            iter != m.end();
            ++iter)
    {
        if (iter->first != iter->second) { diff = iter; }
        MapType::iterator rb_iter = rb->Get(iter->first);
        ASSERT_NE (rb_iter, rb->end());
        EXPECT_EQ (iter->second, rb_iter.Value());
    }
    ASSERT_NE (diff, m.end());
    rb_iter = rb->Get(diff->first);
    rb_iter.Value() = diff->first;
    EXPECT_EQ(rb->Get(diff->first).Value(), diff->first);
}

TEST_F(RBTreeUnittest, Delete)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    {
        boost::scoped_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
        ASSERT_TRUE (rb != NULL);
        EXPECT_TRUE (rb->Put(8, 1));
        EXPECT_TRUE (rb->Put(4, 1));
        EXPECT_TRUE (rb->Put(1, 1));
        EXPECT_TRUE (rb->Put(2, 1));
        EXPECT_TRUE (rb->Put(3, 1));
        EXPECT_TRUE (rb->Put(9, 1));

        EXPECT_EQ (rb->size(), 6);

        EXPECT_EQ (1, rb->Delete(8));
        EXPECT_EQ(true, rb->Put(7, 1));

        EXPECT_EQ (1, rb->Delete(4));
        EXPECT_TRUE (rb->Put(0, 1));

        EXPECT_EQ (1, rb->Delete(1));
        EXPECT_TRUE (rb->Put(6, 1));
        EXPECT_EQ (1, rb->Delete(2));
    }

    {
        boost::scoped_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
        ASSERT_TRUE (rb != NULL);
        EXPECT_TRUE(rb->Put(5, 1));
        EXPECT_TRUE(rb->Put(4, 3));
        EXPECT_TRUE(rb->Put(6, 5));
        EXPECT_TRUE(rb->Put(1, 7) );
        EXPECT_TRUE(rb->Put(7, 9));
        EXPECT_TRUE(rb->Put(2, 11));
        ASSERT_EQ(1, rb->Delete(5));
        EXPECT_EQ (5, rb->size());
        EXPECT_TRUE (rb->Put(3, 14));
        EXPECT_EQ (1, rb->Delete(4));
    }
    {
        boost::scoped_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
        ASSERT_TRUE (rb != NULL);
        rb->Put(866117813, 1);
        rb->Put(1960475568, 3);
        rb->Put(2420802265, 5);
        rb->Put(236576153, 7);
        rb->Put(3980542622, 9);
        rb->Put(2480781215, 11);
        rb->Put(856542758, 13);
        rb->Put(2213215722, 15);
        rb->Put(2016062719, 17);
        rb->Put(2289727763, 19);
        rb->Put(1718045791, 21);
        rb->Put(1013099794, 23);
        rb->Put(3490653748, 25);
        rb->Put(2644540065, 27);
        rb->Put(1984934498, 29);
        rb->Put(3007203938, 31);
        rb->Put(2466680078, 33);
        rb->Put(828258768, 35);
        rb->Put(1770195889, 37);
        rb->Put(1352079799, 39);
        rb->Delete(866117813);
        rb->Put(1378227868, 42);
        rb->Delete(1960475568);
        rb->Put(3678573480, 45);
        rb->Delete(2420802265);
        rb->Put(360248174, 48);
        rb->Delete(236576153);
        rb->Put(1281442473, 51);
        rb->Delete(3980542622);
        rb->Put(770928838, 54);
        rb->Delete(2480781215);
        rb->Put(3124197642, 57);
        rb->Delete(856542758);
        rb->Put(495573184, 60);
        rb->Delete(2213215722);
        rb->Put(305885458, 63);
        rb->Delete(2016062719);
        rb->Put(1387194867, 66);
        rb->Delete(2289727763);
        rb->Put(2015645127, 69);
        rb->Delete(1718045791);
        rb->Put(3924870233, 72);
        rb->Delete(1013099794);
        rb->Put(675386749, 75);
        rb->Delete(3490653748);
        rb->Put(3356572176, 78);
        rb->Delete(2644540065);
        rb->Put(3082068503, 81);
        rb->Delete(1984934498);
        rb->Put(3002900047, 84);
        rb->Delete(3007203938);
        rb->Put(2320199352, 87);
        rb->Delete(2466680078);
        rb->Put(2642441610, 90);
        rb->Delete(828258768);
        rb->Put(4266967236, 93);
        rb->Delete(1770195889);
        rb->Put(4037032887, 96);
        rb->Delete(1352079799);
        rb->Put(1873661176, 99);
        rb->Delete(1378227868);
        rb->Put(2332339523, 102);
        rb->Delete(3678573480);
        rb->Put(2921183345, 105);
        rb->Delete(360248174);
        rb->Put(3247524035, 108);
        rb->Delete(1281442473);
        rb->Put(1947561308, 111);
        rb->Delete(770928838);
        rb->Put(4090995685, 114);
        rb->Delete(3124197642);
        rb->Put(1094503718, 117);
        rb->Delete(495573184);
        rb->Put(716392057, 120);
        rb->Delete(305885458);
        rb->Put(771558417, 123);
        rb->Delete(1387194867);
        rb->Put(3120712721, 126);
        rb->Delete(2015645127);
        rb->Put(1950074739, 129);
        rb->Delete(3924870233);
        rb->Put(1047765460, 132);
        rb->Delete(675386749);
        rb->Put(239334826, 135);
        rb->Delete(3356572176);
        rb->Put(2142359845, 138);
        rb->Delete(3082068503);
        rb->Put(3798596731, 141);
        rb->Delete(3002900047);
        rb->Put(1397595248, 144);
        rb->Delete(2320199352);
        rb->Put(173310247, 147);
        rb->Delete(2642441610);
        rb->Put(3376084667, 150);
        rb->Delete(4266967236);
        rb->Put(3814935247, 153);
        rb->Delete(4037032887);
        rb->Put(2501246673, 156);
        rb->Delete(1873661176);
        rb->Put(3671355826, 159);
        rb->Delete(2332339523);
        rb->Put(1652736259, 162);
        rb->Delete(2921183345);
        rb->Put(367393798, 165);
        rb->Delete(3247524035);
        rb->Put(967066130, 168);
        rb->Delete(1947561308);
        rb->Put(1069738544, 171);
        rb->Delete(4090995685);
        rb->Put(737689458, 174);
        rb->Delete(1094503718);
        rb->Put(3952831028, 177);
        rb->Delete(716392057);
        rb->Put(661940096, 180);
        rb->Delete(771558417);
        rb->Put(4207122610, 183);
        rb->Delete(3120712721);
        rb->Put(536230062, 186);
        rb->Delete(1950074739);
        rb->Put(3877218121, 189);
        rb->Delete(1047765460);
        rb->Put(2859442613, 192);
        rb->Delete(239334826);
        rb->Put(4025259179, 195);
        rb->Delete(2142359845);
        rb->Put(2085828154, 198);
        rb->Delete(3798596731);
        rb->Put(2214297963, 201);
        rb->Delete(1397595248);
        rb->Put(270603657, 204);
        rb->Delete(173310247);
        rb->Put(2837115977, 207);
        rb->Delete(3376084667);
        rb->Put(1777270011, 210);
        rb->Delete(3814935247);
        rb->Put(1609205742, 213);
        rb->Delete(2501246673);
    }
#if 1
    {
        boost::scoped_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
        ASSERT_TRUE (rb != NULL);
        rb->Put(1555047715, 1);
        rb->Put(4134443480, 3);
        rb->Put(1267048824, 5);
        rb->Put(1958422833, 7);
        rb->Put(4183889219, 9);
        rb->Put(1330743630, 11);
        rb->Put(3186634898, 13);
        rb->Put(3672905085, 15);
        rb->Put(3105582269, 17);
        rb->Put(1370412186, 19);
        rb->Put(1508585981, 21);
        rb->Put(2348675376, 23);
        rb->Put(3595798341, 25);
        rb->Put(1197120343, 27);
        rb->Put(2938339437, 29);
        rb->Put(2545098921, 31);
        rb->Put(2173391100, 33);
        rb->Put(3635692033, 35);
        rb->Put(3690175769, 38);
        rb->Put(4042586451, 40);
        rb->Put(4077367769, 42);
        rb->Put(786573570, 44);
        rb->Put(2599248208, 46);
        rb->Put(3240250351, 48);
        rb->Put(3854502615, 50);
        rb->Put(3841496952, 52);
        rb->Put(3584917595, 54);
        rb->Put(2417692625, 56);
        rb->Put(3472875832, 58);
        rb->Put(473275425, 60);
        rb->Put(2230093304, 62);
        rb->Put(220393561, 64);
        rb->Put(3780883109, 66);
        rb->Put(2005416986, 68);
        rb->Put(3102445184, 70);
        rb->Put(1452982982, 72);
        rb->Put(2478177449, 74);
        rb->Put(4045210274, 76);
        rb->Put(2610220979, 78);
        rb->Put(4117534683, 80);
        rb->Put(3176700881, 82);
        rb->Put(109641054, 84);
        rb->Put(3720692166, 86);
        rb->Put(2166523361, 88);
        rb->Put(1058087494, 90);
        rb->Put(2015509281, 92);
        rb->Put(802374628, 94);
        rb->Put(2329820162, 96);
        rb->Put(816133341, 98);
        rb->Put(2936798126, 100);
        rb->Put(549050062, 102);
        rb->Put(1162295470, 104);
        rb->Put(1496101821, 106);
        rb->Put(2873875326, 108);
        rb->Put(3492351778, 110);
        rb->Put(933476535, 112);
        rb->Put(199890938, 114);
        rb->Put(288644546, 116);
        rb->Put(1176593883, 118);
        rb->Put(2621855701, 120);
        rb->Put(273583799, 122);
        rb->Put(1313576555, 124);
        rb->Put(4022546032, 126);
        rb->Put(3473652185, 128);
        rb->Put(2139370440, 130);
        rb->Put(1218704449, 132);
        rb->Put(1817929023, 134);
        rb->Put(901756561, 136);
        rb->Put(2622434176, 138);
        rb->Put(3954839286, 140);
        rb->Put(3148417703, 142);
        rb->Put(1455321598, 144);
        rb->Put(893871486, 146);
        rb->Put(4262833158, 148);
        rb->Put(1917790275, 150);
        rb->Put(680183052, 152);
        rb->Put(3576227631, 154);
        rb->Put(346811849, 156);
        rb->Put(1081809842, 158);
        rb->Put(4183046740, 160);
        rb->Put(3488603641, 162);
        rb->Put(2779883168, 164);
        rb->Put(1033826129, 166);
        rb->Put(3332055273, 168);
        rb->Put(3043553837, 170);
        rb->Put(583928918, 172);
        rb->Put(3287832932, 174);
        rb->Put(2973446173, 176);
        rb->Put(2574118671, 178);
        rb->Put(3703347883, 180);
        rb->Put(2171591390, 182);
        rb->Put(1356167412, 184);
        rb->Put(3287044635, 186);
        rb->Put(3262542286, 188);
        rb->Put(3827664983, 190);
        rb->Put(2487161696, 192);
        rb->Put(1535747026, 194);
        rb->Put(64315796, 196);
        rb->Put(2657333450, 198);
        rb->Put(1683619532, 200);
        rb->Put(3414654512, 202);
        rb->Put(2657366416, 204);
        rb->Put(2020342763, 206);
        rb->Put(4230506382, 208);
        rb->Put(1930748707, 210);
        rb->Put(1370197060, 212);
        rb->Put(4272941972, 214);
        rb->Put(2391334260, 216);
        rb->Put(1186674721, 218);
        rb->Put(3809825220, 220);
        rb->Put(3445161888, 222);
        rb->Put(1291296250, 224);
        rb->Put(4040518799, 226);
        rb->Put(2381867905, 228);
        rb->Put(2784531453, 230);
        rb->Put(2486843852, 232);
        rb->Put(2815724674, 234);
        rb->Put(292807120, 236);
        rb->Put(2360015633, 238);
        rb->Put(2368150552, 240);
        rb->Put(785434832, 242);
        rb->Put(4247048551, 244);
        rb->Put(2212416565, 246);
        rb->Put(1839462683, 248);
        rb->Put(265309129, 250);
        rb->Put(4233467202, 252);
        rb->Put(1374262967, 254);
        rb->Put(383462315, 256);
        rb->Put(2576377615, 258);
        rb->Put(2642316577, 260);
        rb->Put(1717043780, 262);
        rb->Put(2090732454, 264);
        rb->Put(1105976566, 266);
        rb->Put(3827607597, 268);
        rb->Put(1762746429, 270);
        rb->Put(826921483, 272);
        rb->Put(2769130304, 274);
        rb->Put(3489052985, 276);
        rb->Put(1757400267, 278);
        rb->Put(3880610839, 280);
        rb->Put(4033459634, 282);
        rb->Put(3991315452, 284);
        rb->Put(3712581399, 286);
        rb->Put(1892309976, 288);
        rb->Put(2016323175, 290);
        rb->Put(2214299421, 292);
        rb->Put(4251576822, 294);
        rb->Put(1189825624, 296);
        rb->Put(2155064341, 298);
        rb->Put(1740113828, 300);
        rb->Put(1237910579, 302);
        rb->Put(3925044136, 304);
        rb->Put(1550019237, 306);
        rb->Put(3304476036, 308);
        rb->Put(2533068027, 310);
        rb->Put(2966407809, 312);
        rb->Put(313246899, 314);
        rb->Put(910161527, 316);
        rb->Put(1935086427, 318);
        rb->Put(3040228823, 320);
        rb->Put(1451925681, 322);
        rb->Put(1329099337, 324);
        rb->Put(1374498275, 326);
        rb->Put(3659543036, 328);
        rb->Put(3284786941, 330);
        rb->Put(3165578761, 332);
        rb->Put(3060461577, 334);
        rb->Put(1743844020, 336);
        rb->Put(1727530365, 338);
        rb->Put(3670408298, 340);
        rb->Put(168127922, 342);
        rb->Put(3047909995, 344);
        rb->Put(384509241, 346);
        rb->Put(4042849745, 348);
        rb->Put(1722123436, 350);
        rb->Put(2609699720, 352);
        rb->Put(2944583432, 354);
        rb->Put(1958823242, 356);
        rb->Put(4245048027, 358);
        rb->Put(3743769134, 360);
        rb->Put(2727653686, 362);
        rb->Put(1769453166, 364);
        rb->Put(3780445963, 366);
        rb->Put(1631071020, 368);
        rb->Put(3283314925, 370);
        rb->Put(3887910325, 372);
        rb->Put(724482359, 374);
        rb->Put(2808739109, 376);
        rb->Put(2701621691, 378);
        rb->Put(2138092567, 380);
        rb->Put(178344385, 382);
        rb->Put(1582717843, 384);
        rb->Put(2553442877, 386);
        rb->Put(3033592791, 388);
        rb->Put(2420079107, 390);
        rb->Put(1639636643, 392);
        rb->Put(3637355806, 394);
        rb->Put(618538599, 396);
        rb->Put(2637903127, 398);
        rb->Put(3055723068, 400);
        rb->Delete(1555047715);
        rb->Put(1729013386, 403);
        rb->Delete(4134443480);
        rb->Put(3558599828, 406);
        rb->Delete(1267048824);
        rb->Put(2725802685, 409);
        rb->Delete(4183889219);
        rb->Put(463597652, 412);
        rb->Delete(1330743630);
        rb->Put(935095958, 415);
        rb->Delete(3186634898);
        rb->Put(4057567218, 418);
        rb->Delete(3672905085);
        rb->Put(1205188109, 421);
        rb->Delete(3105582269);
        rb->Put(2697182286, 424);
        rb->Delete(1370412186);
        rb->Put(3210033585, 427);
        rb->Delete(1508585981);
        rb->Put(3946958879, 430);
        rb->Delete(2348675376);
        rb->Put(1141895424, 433);
        rb->Delete(3595798341);
        rb->Put(1514867190, 436);
        rb->Delete(1197120343);
        rb->Put(1027670031, 439);
        rb->Delete(2938339437);
        rb->Put(274408545, 442);
        rb->Delete(2545098921);
        rb->Put(1939406951, 445);
        rb->Delete(2173391100);
        rb->Put(1014261796, 448);
        rb->Delete(3635692033);
        rb->Put(541146406, 451);
        rb->Delete(1958422833);
        rb->Put(4281731029, 454);
        rb->Delete(3690175769);
        rb->Put(1738841933, 457);
        rb->Delete(4042586451);
        rb->Put(873086969, 460);
        rb->Delete(4077367769);
        rb->Put(205706645, 463);
        rb->Delete(786573570);
        rb->Put(1020158874, 466);
        rb->Delete(2599248208);
        rb->Put(3803350123, 469);
        rb->Delete(3240250351);
        rb->Put(4088559245, 472);
        rb->Delete(3854502615);
        rb->Put(1456609581, 475);
        rb->Delete(3841496952);
        rb->Put(3726052804, 478);
        rb->Delete(3584917595);
        rb->Put(2150337446, 481);
        rb->Delete(2417692625);
        rb->Put(11245074, 484);
        rb->Delete(3472875832);
        rb->Put(3598983925, 487);
        rb->Delete(473275425);
        rb->Put(201606781, 490);
        rb->Delete(2230093304);
        rb->Put(485235743, 493);
        rb->Delete(220393561);
        rb->Put(168649042, 496);
        rb->Delete(3780883109);
        rb->Put(768265469, 499);
        rb->Delete(2005416986);
        rb->Put(1209833682, 502);
        rb->Delete(3102445184);
        rb->Put(1070527749, 505);
        rb->Delete(1452982982);
        rb->Put(2167003677, 508);
        rb->Delete(2478177449);
        rb->Put(4279148184, 511);
        rb->Delete(4045210274);
        rb->Put(3006675348, 514);
        rb->Delete(2610220979);
        rb->Put(1729554639, 517);
        rb->Delete(4117534683);
        rb->Put(1810115961, 520);
        rb->Delete(3176700881);
        rb->Put(2621760329, 523);
        rb->Delete(109641054);
        rb->Put(2847582915, 526);
        rb->Delete(3720692166);
        rb->Put(3556335471, 529);
        rb->Delete(2166523361);
        rb->Put(214228542, 532);
        rb->Delete(1058087494);
        rb->Put(1018512269, 535);
        rb->Delete(2015509281);
        rb->Put(1447551300, 538);
        rb->Delete(802374628);
        rb->Put(1419940560, 541);
        rb->Delete(2329820162);
        rb->Put(2196386742, 544);
        rb->Delete(816133341);
        rb->Put(2247891303, 547);
        rb->Delete(2936798126);
        rb->Put(301653126, 550);
        rb->Delete(549050062);
        rb->Put(4279871547, 553);
        rb->Delete(1162295470);
        rb->Put(544809321, 556);
        rb->Delete(1496101821);
        rb->Put(1536454659, 559);
        rb->Delete(2873875326);
        rb->Put(1858249158, 562);
        rb->Delete(3492351778);
        rb->Put(3503912077, 565);
        rb->Delete(933476535);
        rb->Put(1139354404, 568);
        rb->Delete(199890938);
        rb->Put(1760333883, 571);
        rb->Delete(288644546);
        rb->Put(326519098, 574);
        rb->Delete(1176593883);
        rb->Put(1552269755, 577);
        rb->Delete(2621855701);
        rb->Put(796043597, 580);
        rb->Delete(273583799);
        rb->Put(546712450, 583);
        rb->Delete(1313576555);
        rb->Put(900901013, 586);
        rb->Delete(4022546032);
        rb->Put(531335347, 589);
        rb->Delete(3473652185);
        rb->Put(817203701, 592);
        rb->Delete(2139370440);
        rb->Put(3931050222, 595);
        rb->Delete(1218704449);
        rb->Put(2887443492, 598);
        rb->Delete(1817929023);
        rb->Put(4186442258, 601);
        rb->Delete(901756561);
        rb->Put(3707646051, 604);
        rb->Delete(2622434176);
        rb->Put(3979163548, 607);
        rb->Delete(3954839286);
        rb->Put(4122182807, 610);
        rb->Delete(3148417703);
        rb->Put(3658494656, 613);
        rb->Delete(1455321598);
        rb->Put(3991956831, 616);
        rb->Delete(893871486);
        rb->Put(58050018, 619);
        rb->Delete(4262833158);
        rb->Put(175033657, 622);
        rb->Delete(1917790275);
        rb->Put(2317116769, 625);
        rb->Delete(680183052);
        rb->Put(922207127, 628);
        rb->Delete(3576227631);
        rb->Put(1649057004, 631);
        rb->Delete(346811849);
        rb->Put(3022362022, 634);
        rb->Delete(1081809842);
        rb->Put(3724319706, 637);
        rb->Delete(4183046740);
        rb->Put(4233093324, 640);
        rb->Delete(3488603641);
        rb->Put(1973478970, 643);
        rb->Delete(2779883168);
        rb->Put(4124642403, 646);
        rb->Delete(1033826129);
        rb->Put(367467564, 649);
        rb->Delete(3332055273);
        rb->Put(3351939001, 652);
        rb->Delete(3043553837);
        rb->Put(82844055, 655);
        rb->Delete(583928918);
        rb->Put(1117924218, 658);
        rb->Delete(3287832932);
        rb->Put(3095495777, 661);
        rb->Delete(2973446173);
        rb->Put(1380327408, 664);
        rb->Delete(2574118671);
        rb->Put(840050972, 667);
        rb->Delete(3703347883);
        rb->Put(15892587, 670);
        rb->Delete(2171591390);
        rb->Put(1805158674, 673);
        rb->Delete(1356167412);
        rb->Put(1369324832, 676);
        rb->Delete(3287044635);
        rb->Put(1670622797, 679);
        rb->Delete(3262542286);
        rb->Put(3932445654, 682);
        rb->Delete(3827664983);
        rb->Put(1799922534, 685);
        rb->Delete(2487161696);
        rb->Put(231725296, 688);
        rb->Delete(1535747026);
        rb->Put(2624282767, 691);
        rb->Delete(64315796);
        rb->Put(2966369020, 694);
        rb->Delete(2657333450);
        rb->Put(3936054030, 697);
        rb->Delete(1683619532);
        rb->Put(3498241063, 700);
        rb->Delete(3414654512);
        rb->Put(517765643, 703);
        rb->Delete(2657366416);
        rb->Put(499938063, 706);
        rb->Delete(2020342763);
        rb->Put(417768089, 709);
        rb->Delete(4230506382);
        rb->Put(3020070830, 712);
        rb->Delete(1930748707);
        rb->Put(2747354418, 715);
        rb->Delete(1370197060);
        rb->Put(3330741564, 718);
        rb->Delete(4272941972);
        rb->Put(3878632921, 721);
        rb->Delete(2391334260);
        rb->Put(1017001350, 724);
        rb->Delete(1186674721);
        rb->Put(1017160976, 727);
        rb->Delete(3809825220);
        rb->Put(974153499, 730);
        rb->Delete(3445161888);
        rb->Put(1453913924, 733);
        rb->Delete(1291296250);
        rb->Put(1671376247, 736);
        rb->Delete(4040518799);
        rb->Put(3912539089, 739);
        rb->Delete(2381867905);
        rb->Put(103412204, 742);
        rb->Delete(2784531453);
        rb->Put(2549374078, 745);
        rb->Delete(2486843852);
        rb->Put(2375599813, 748);
        rb->Delete(2815724674);
        rb->Put(4128372216, 751);
        rb->Delete(292807120);
        rb->Put(2945215302, 754);
        rb->Delete(2360015633);
        rb->Put(234252031, 757);
        rb->Delete(2368150552);
        rb->Put(280312335, 760);
        rb->Delete(785434832);
        rb->Put(2623575876, 763);
        rb->Delete(4247048551);
        rb->Put(3764903714, 766);
        rb->Delete(2212416565);
        rb->Put(2446805838, 769);
        rb->Delete(1839462683);
        rb->Put(1164751596, 772);
        rb->Delete(265309129);
        rb->Put(567813220, 775);
        rb->Delete(4233467202);
        rb->Put(2677019910, 778);
        rb->Delete(1374262967);
        rb->Put(59489274, 781);
        rb->Delete(383462315);
        rb->Put(3502306760, 784);
        rb->Delete(2576377615);
        rb->Put(3404563608, 787);
        rb->Delete(2642316577);
        rb->Put(3464288132, 790);
        rb->Delete(1717043780);
        rb->Put(2841683230, 793);
        rb->Delete(2090732454);
        rb->Put(685136330, 796);
        rb->Delete(1105976566);
        rb->Put(1827371838, 799);
        rb->Delete(3827607597);
        rb->Put(216055609, 802);
        rb->Delete(1762746429);
        rb->Put(4267185362, 805);
        rb->Delete(826921483);
        rb->Put(1153699602, 808);
        rb->Delete(2769130304);
        rb->Put(1912109266, 811);
        rb->Delete(3489052985);
        rb->Put(2521490556, 814);
        rb->Delete(1757400267);
        rb->Put(2244667737, 817);
        rb->Delete(3880610839);
        rb->Put(3771877818, 820);
        rb->Delete(4033459634);
        rb->Put(475299747, 823);
        rb->Delete(3991315452);
        rb->Put(2620333168, 826);
        rb->Delete(3712581399);
        rb->Put(1870102282, 829);
        rb->Delete(1892309976);
        rb->Put(1594723424, 832);
        rb->Delete(2016323175);
        rb->Put(3613059114, 835);
        rb->Delete(2214299421);
        rb->Put(209726652, 838);
        rb->Delete(4251576822);
        rb->Put(440201645, 841);
        rb->Delete(1189825624);
        rb->Put(3803422521, 844);
        rb->Delete(2155064341);
        rb->Put(3657137031, 847);
        rb->Delete(1740113828);
        rb->Put(1121510668, 850);
        rb->Delete(1237910579);
        rb->Put(2594294970, 853);
        rb->Delete(3925044136);
        rb->Put(3413631395, 856);
        rb->Delete(1550019237);
        rb->Put(345418178, 859);
        rb->Delete(3304476036);
        rb->Put(695523196, 862);
        rb->Delete(2533068027);
        rb->Put(2048625817, 865);
        rb->Delete(2966407809);
        rb->Put(1171841695, 868);
        rb->Delete(313246899);
        rb->Put(3137977355, 871);
        rb->Delete(910161527);
        rb->Put(326607223, 874);
        rb->Delete(1935086427);
        rb->Put(869895025, 877);
        rb->Delete(3040228823);
        rb->Put(3319780430, 880);
        rb->Delete(1451925681);
        rb->Put(3982580554, 883);
        rb->Delete(1329099337);
        rb->Put(1859045818, 886);
        rb->Delete(1374498275);
        rb->Put(1901450431, 889);
        rb->Delete(3659543036);
        rb->Put(2880602698, 892);
        rb->Delete(3284786941);
        rb->Put(656296256, 895);
        rb->Delete(3165578761);
        rb->Put(3610888242, 898);
        rb->Delete(3060461577);
        rb->Put(3562947437, 901);
        rb->Delete(1743844020);
        rb->Put(2807658132, 904);
        rb->Delete(1727530365);
        rb->Put(1362821289, 907);
        rb->Delete(3670408298);
        rb->Put(1091261041, 910);
        rb->Delete(168127922);
        rb->Put(1736199921, 913);
        rb->Delete(3047909995);
        rb->Put(302335842, 916);
        rb->Delete(384509241);
        rb->Put(3966115217, 919);
        rb->Delete(4042849745);
        rb->Put(1596364379, 922);
        rb->Delete(1722123436);
        rb->Put(3325982825, 925);
        rb->Delete(2609699720);
        rb->Put(3935779388, 928);
        rb->Delete(2944583432);
        rb->Put(979566780, 931);
        rb->Delete(1958823242);
        rb->Put(1098391320, 934);
        rb->Delete(4245048027);
        rb->Put(776817788, 937);
        rb->Delete(3743769134);
        rb->Put(3721768627, 940);
        rb->Delete(2727653686);
        rb->Put(4044448551, 943);
        rb->Delete(1769453166);
        rb->Put(4090328963, 946);
        rb->Delete(3780445963);
        rb->Put(3098637949, 949);
        rb->Delete(1631071020);
        rb->Put(1350368446, 952);
        rb->Delete(3283314925);
        rb->Put(3945964756, 955);
        rb->Delete(3887910325);
        rb->Put(500219422, 958);
        rb->Delete(724482359);
        rb->Put(3864882528, 961);
        rb->Delete(2808739109);
        rb->Put(4247434967, 964);
        rb->Delete(2701621691);
        rb->Put(1191725540, 967);
        rb->Delete(2138092567);
        rb->Put(2341750375, 970);
        rb->Delete(178344385);
        rb->Put(952338378, 973);
        rb->Delete(1582717843);
        rb->Put(912976946, 976);
        rb->Delete(2553442877);
        rb->Put(183633475, 979);
        rb->Delete(3033592791);
        rb->Put(1560951310, 982);
        rb->Delete(2420079107);
        rb->Put(2185423304, 985);
        rb->Delete(1639636643);
        rb->Put(849220605, 988);
        rb->Delete(3637355806);
        rb->Put(1977246544, 991);
        rb->Delete(618538599);
        rb->Put(61220082, 994);
        rb->Delete(2637903127);
        rb->Put(1261118680, 997);
        rb->Delete(3055723068);
        rb->Put(1870260947, 1000);
        rb->Delete(1729013386);
        rb->Put(1898830847, 1003);
        rb->Delete(3558599828);
        rb->Put(2292453054, 1006);
        rb->Delete(2725802685);
        rb->Put(1563808743, 1009);
        rb->Delete(463597652);
        rb->Put(678003589, 1012);
        rb->Delete(935095958);
        rb->Put(3862114077, 1015);
        rb->Delete(4057567218);
        rb->Put(649408417, 1018);
        rb->Delete(1205188109);
        rb->Put(4145343894, 1021);
        rb->Delete(2697182286);
        rb->Put(1959371866, 1024);
        rb->Delete(3210033585);
        rb->Put(1881003435, 1027);
        rb->Delete(3946958879);
        rb->Put(4097442828, 1030);
        rb->Delete(1141895424);
        rb->Put(3014497249, 1033);
        rb->Delete(1514867190);
        rb->Put(2865558489, 1036);
        rb->Delete(1027670031);
        rb->Put(2460577716, 1039);
        rb->Delete(274408545);
        rb->Put(2042240725, 1042);
        rb->Delete(1939406951);
        rb->Put(2463598882, 1045);
        rb->Delete(1014261796);
        rb->Put(1240246141, 1048);
        rb->Delete(541146406);
        rb->Put(3520384036, 1051);
        rb->Delete(4281731029);
        rb->Put(1732208477, 1054);
        rb->Delete(1738841933);
        rb->Put(1553218540, 1057);
        rb->Delete(873086969);
        rb->Put(3690056963, 1060);
        rb->Delete(205706645);
        rb->Put(451573860, 1063);
        rb->Delete(1020158874);
        rb->Put(2769526448, 1066);
        rb->Delete(3803350123);
        rb->Put(917065573, 1069);
        rb->Delete(4088559245);
        rb->Put(1998610324, 1072);
        rb->Delete(1456609581);
        rb->Put(2895630941, 1075);
        rb->Delete(3726052804);
        rb->Put(746691041, 1078);
        rb->Delete(2150337446);
        rb->Put(932539525, 1081);
        rb->Delete(11245074);
        rb->Put(1671253880, 1084);
        rb->Delete(3598983925);
        rb->Put(1952436754, 1087);
        rb->Delete(201606781);
        rb->Put(2292730466, 1090);
        rb->Delete(485235743);
        rb->Put(4109050486, 1093);
        rb->Delete(168649042);
        rb->Put(2543135791, 1096);
        rb->Delete(768265469);
        rb->Put(2307089809, 1099);
        rb->Delete(1209833682);
        rb->Put(3352853400, 1102);
        rb->Delete(1070527749);
        rb->Put(1049562973, 1105);
        rb->Delete(2167003677);
        rb->Put(4168425581, 1108);
        rb->Delete(4279148184);
        rb->Put(113728937, 1111);
        rb->Delete(3006675348);
        rb->Put(1426178622, 1114);
        rb->Delete(1729554639);
        rb->Put(1252331764, 1117);
        rb->Delete(1810115961);
        rb->Put(1883371506, 1120);
        rb->Delete(2621760329);
        rb->Put(301033043, 1123);
        rb->Delete(2847582915);
        rb->Put(35203216, 1126);
        rb->Delete(3556335471);
        rb->Put(1723203861, 1129);
        rb->Delete(214228542);
        rb->Put(294726511, 1132);
        rb->Delete(1018512269);
        rb->Put(2978152476, 1135);
        rb->Delete(1447551300);
        rb->Put(27581277, 1138);
        rb->Delete(1419940560);
        rb->Put(467854729, 1141);
        rb->Delete(2196386742);
        rb->Put(3716782173, 1144);
        rb->Delete(2247891303);
        rb->Put(1389708028, 1147);
        rb->Delete(301653126);
        rb->Put(1561123018, 1150);
        rb->Delete(4279871547);
        rb->Put(267292854, 1153);
        rb->Delete(544809321);
        rb->Put(1690772620, 1156);
        rb->Delete(1536454659);
        rb->Put(1837477330, 1159);
        rb->Delete(1858249158);
        rb->Put(1856231484, 1162);
        rb->Delete(3503912077);
        rb->Put(2220427134, 1165);
        rb->Delete(1139354404);
        rb->Put(3912374530, 1168);
        rb->Delete(1760333883);
        rb->Put(1352312094, 1171);
        rb->Delete(326519098);
        rb->Put(1078084986, 1174);
        rb->Delete(1552269755);
        rb->Put(4198680974, 1177);
        rb->Delete(796043597);
        rb->Put(1422459708, 1180);
        rb->Delete(546712450);
        rb->Put(2811978546, 1183);
        rb->Delete(900901013);
        rb->Put(2643768465, 1186);
        rb->Delete(531335347);
        rb->Put(2313345537, 1189);
        rb->Delete(817203701);
        rb->Put(3265103064, 1192);
        rb->Delete(3931050222);
        rb->Put(1203376123, 1195);
        rb->Delete(2887443492);
        rb->Put(998355167, 1198);
        rb->Delete(4186442258);
        rb->Put(418626871, 1201);
        rb->Delete(3707646051);
        rb->Put(1312107592, 1204);
        rb->Delete(3979163548);
        rb->Put(3370393042, 1207);
        rb->Delete(4122182807);
        rb->Put(3365621792, 1210);
        rb->Delete(3658494656);
        rb->Put(1691895837, 1213);
        rb->Delete(3991956831);
        rb->Put(2055414644, 1216);
        rb->Delete(58050018);
        rb->Put(3981238486, 1219);
        rb->Delete(175033657);
        rb->Put(1197525978, 1222);
        rb->Delete(2317116769);
        rb->Put(2344750536, 1225);
        rb->Delete(922207127);
        rb->Put(1190144530, 1228);
        rb->Delete(1649057004);
        rb->Put(1167954651, 1231);
        rb->Delete(3022362022);
        rb->Put(217121005, 1234);
        rb->Delete(3724319706);
        rb->Put(2595032327, 1237);
        rb->Delete(4233093324);
        rb->Put(3592683008, 1240);
        rb->Delete(1973478970);
        rb->Put(3615297829, 1243);
        rb->Delete(4124642403);
    }
#endif
}

TEST_F(RBTreeUnittest, Clear)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    boost::scoped_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (rb != NULL);

    while (1)
    {
        unsigned key, val;
        key = val = rand();
        bool ok = rb->Put(key, val);
        if (not ok) break;
    }
    ASSERT_EQ (rb->size(), rb->capacity());
    ASSERT_TRUE (false == rb->Put(1, 2));
    rb->Clear();
    EXPECT_EQ (rb->size(), 0);
    EXPECT_EQ (rb->depth(), 0);
    EXPECT_TRUE (rb->Put(1, 2));
}

TEST_F(RBTreeUnittest, iterator)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    boost::scoped_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (rb != NULL);

    EXPECT_EQ (rb->begin(), rb->end());
    for (unsigned i = 0; ; ++i)
    {
        if (rb->Put(i, i) == false) break;
    }
    EXPECT_EQ (2045, rb->size());

    unsigned idx = 0;
    for (MapType::iterator iter = rb->begin(); iter != rb->end(); ++iter)
    {
        ASSERT_EQ (idx, iter.Key());
        ASSERT_EQ (idx, iter.Value());
        iter.Value() = idx + 1;
        ++idx;
    }

    idx = 0;
    for (MapType::iterator iter = rb->begin(); iter != rb->end(); ++iter)
    {
        ASSERT_EQ (idx, iter.Key());
        ASSERT_EQ (idx+1, iter.Value());
        ++idx;
    }
}

TEST_F(RBTreeUnittest, const_iterator)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    boost::scoped_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (rb != NULL);

    const MapType * const_tree = rb.get();
    EXPECT_EQ (const_tree->begin(), const_tree->end());
    rb->Put(1, 2);
    rb->Put(3, 4);
    EXPECT_NE (const_tree->begin(), const_tree->end());
    MapType::const_iterator iter = const_tree->Get(3);
    EXPECT_EQ (iter.Value(), 4);
    iter = rb->begin();
}

TEST_F(RBTreeUnittest, RestoreFrom)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    std::map<unsigned, unsigned> m;
    {
        boost::scoped_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
        ASSERT_TRUE (rb != NULL);

        while (1)
        {
            unsigned key, val;
            key = rand();
            val = rand();
            if (not rb->Put(key, val)) break;
            m[key] = val;
        }
        EXPECT_EQ (m.size(), rb->size());
        EXPECT_LE (rb->depth(), 10);
    }
    char newbuf[kBufSize * 2];                  /* larger buffer */
    ::memcpy(newbuf, buf, sizeof(buf));
    {
        boost::scoped_ptr< MapType > rb(MapType::RestoreFrom(newbuf, sizeof(newbuf)));
        ASSERT_TRUE (rb != NULL);
        EXPECT_EQ (m.size(), rb->size());
        for (std::map<unsigned, unsigned>::const_iterator iter = m.begin();
                iter != m.end();
                ++iter)
        {
            EXPECT_EQ (iter->second, rb->Get(iter->first).Value());
        }
    }
}
