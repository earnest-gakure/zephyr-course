/*
 * Ring Buffer Module - Homework Test Skeleton
 *
 * test_fresh_state is provided as a worked example. Fill in the remaining
 * 7 ZTEST bodies according to TEST_SPEC.md. Stubs call ztest_test_skip()
 * so the binary builds and runs cleanly before each test is implemented.
 *
 * Run:
 *   west twister -T tests/ring_buf -p native_sim
 */

#include <zephyr/ztest.h>
#include <errno.h>

#include "ring_buf.h"

/*
 * Shared before hook: every suite reinitialises the ring buffer with a
 * capacity of 4 so tests start from a clean, known state.
 */
static void before(void *f)
{
        ARG_UNUSED(f);
        rb_init(4);
}

/*
 * ============================================================================
 * Test Suite: ring_buf_init
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_init, NULL, NULL, before, NULL, NULL);

/* PROVIDED — study this test before writing the rest. */
ZTEST(ring_buf_init, test_fresh_state)
{
        zassert_true(rb_is_empty(), "Fresh buffer must be empty");
        zassert_equal(rb_count(), 0, "Fresh buffer count must be 0");
}

ZTEST(ring_buf_init, test_reinit_clears_state)
{
        rb_push(99);
        rb_init(4);
        zassert_true(rb_is_empty(), "Buffer must be empty after reinit");
        zassert_equal(rb_count(), 0, "Count must be 0 after reinit");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_push_pop
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_push_pop, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_push_pop, test_single_push_pop)
{
        int v;

        zassert_equal(rb_push(42), 0, "push(42) must return 0");
        zassert_equal(rb_pop(&v), 0, "pop must return 0");
        zassert_equal(v, 42, "popped value must be 42");
        zassert_true(rb_is_empty(), "Buffer must be empty after pop");
}

ZTEST(ring_buf_push_pop, test_fifo_order)
{
        int v;

        zassert_equal(rb_push(1), 0, "push(1) must return 0");
        zassert_equal(rb_push(2), 0, "push(2) must return 0");
        zassert_equal(rb_push(3), 0, "push(3) must return 0");

        zassert_equal(rb_pop(&v), 0, "first pop must return 0");
        zassert_equal(v, 1, "first pop must yield 1");

        zassert_equal(rb_pop(&v), 0, "second pop must return 0");
        zassert_equal(v, 2, "second pop must yield 2");

        zassert_equal(rb_pop(&v), 0, "third pop must return 0");
        zassert_equal(v, 3, "third pop must yield 3");

        zassert_true(rb_is_empty(), "Buffer must be empty after all pops");
}

ZTEST(ring_buf_push_pop, test_push_full_returns_enospc)
{
        zassert_equal(rb_push(1), 0, "push(1) must return 0");
        zassert_equal(rb_push(2), 0, "push(2) must return 0");
        zassert_equal(rb_push(3), 0, "push(3) must return 0");
        zassert_equal(rb_push(4), 0, "push(4) must return 0");
        zassert_true(rb_is_full(), "Buffer must be full after 4 pushes");

        zassert_equal(rb_push(99), -ENOSPC, "push on full buffer must return -ENOSPC");
        zassert_equal(rb_count(), 4, "Count must remain 4 after rejected push");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_boundaries
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_boundaries, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_boundaries, test_peek_does_not_consume)
{
        int v;

        zassert_equal(rb_push(7), 0, "push(7) must return 0");

        zassert_equal(rb_peek(&v), 0, "first peek must return 0");
        zassert_equal(v, 7, "first peek must yield 7");

        zassert_equal(rb_peek(&v), 0, "second peek must return 0");
        zassert_equal(v, 7, "second peek must yield 7");

        zassert_equal(rb_count(), 1, "count must still be 1 after two peeks");
}

ZTEST(ring_buf_boundaries, test_pop_null_returns_einval)
{
        zassert_equal(rb_pop(NULL), -EINVAL, "pop(NULL) must return -EINVAL");
}

ZTEST(ring_buf_boundaries, test_is_full_after_fill)
{
        zassert_equal(rb_push(1), 0, "push(1) must return 0");
        zassert_equal(rb_push(2), 0, "push(2) must return 0");
        zassert_equal(rb_push(3), 0, "push(3) must return 0");
        zassert_equal(rb_push(4), 0, "push(4) must return 0");
        zassert_true(rb_is_full(), "Buffer must be full after 4 pushes");
        zassert_equal(rb_count(), 4, "Count must be 4 after fill");
}
