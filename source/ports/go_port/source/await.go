package metacall

/*
extern void *goResolve(void *, void *);
extern void *goReject(void *, void *);

void *resolveCgo(void *v, void *ctx) {
	return goResolve(v, ctx);
}

void *rejectCgo(void *v, void *ctx) {
	return goReject(v, ctx);
}
*/
import "C"
