; ModuleID = 'ptoas.hivm.official.vector'
source_filename = "ptoas.hivm.official.vector"

declare void @llvm.hivm.store.vfsimt.info(i64)

declare void @llvm.hivm.SET.FLAG.IMM(i64, i64, i64)

declare void @llvm.hivm.WAIT.FLAG.IMM(i64, i64, i64)

declare void @llvm.hivm.MOV.UB.TO.OUT.ALIGN.V2.DV(ptr addrspace(1), ptr addrspace(6), i64, i64)

declare void @llvm.hivm.BARRIER(i64)

declare i32 @llvm.hivm.get.TID.X()

declare i32 @llvm.hivm.get.laneID()

declare i32 @llvm.hivm.get.BLOCK.DIM.X()

declare i32 @llvm.hivm.get.BLOCK.DIM.Y()

declare i32 @llvm.hivm.get.BLOCK.DIM.Z()

declare i32 @llvm.hivm.get.GRID.DIM.X()

declare i32 @llvm.hivm.get.GRID.DIM.Y()

declare i32 @llvm.hivm.get.GRID.DIM.Z()

declare i32 @llvm.hivm.get.BLOCK.IDX.X()

declare i32 @llvm.hivm.get.LANEMASK.EQ()

declare i32 @llvm.hivm.get.LANEMASK.LE()

declare i32 @llvm.hivm.get.LANEMASK.LT()

declare i32 @llvm.hivm.get.LANEMASK.GE()

declare i32 @llvm.hivm.get.LANEMASK.GT()

declare i1 @llvm.hivm.vote.all(i1)

declare i1 @llvm.hivm.vote.any(i1)

declare i1 @llvm.hivm.vote.uni(i1)

declare i32 @llvm.hivm.vote.ballot(i1)

declare i32 @llvm.hivm.shfl.idx.i32(i32, i32)

declare i32 @llvm.hivm.shfl.up.i32(i32, i32)

declare i32 @llvm.hivm.shfl.down.i32(i32, i32)

declare i32 @llvm.hivm.shfl.bfly.i32(i32, i32)

declare i32 @llvm.hivm.redux.add.s32(i32)

declare i32 @llvm.hivm.redux.max.s32(i32)

declare i32 @llvm.hivm.redux.min.s32(i32)

declare float @llvm.hivm.redux.add.f32(float)

declare i32 @llvm.hivm.fp32.to.s32(float, i32, i32)

define void @simt_scalar_core_kernel_mix_aiv(ptr addrspace(1) %0) #0 {
  call void @llvm.hivm.store.vfsimt.info(i64 4295032864)
  call simt_entry void @simt_scalar_core_body(ptr addrspace(6) null)
  call void @llvm.hivm.SET.FLAG.IMM(i64 1, i64 5, i64 0)
  call void @llvm.hivm.WAIT.FLAG.IMM(i64 1, i64 5, i64 0)
  call void @llvm.hivm.MOV.UB.TO.OUT.ALIGN.V2.DV(ptr addrspace(1) %0, ptr addrspace(6) null, i64 4294967808, i64 140737488355456)
  call void @llvm.hivm.BARRIER(i64 6)
  ret void
}

; Function Attrs: noinline
define linkonce_odr simt_entry void @simt_scalar_core_body(ptr addrspace(6) %0) #1 !annotation !6 !annotation !7 {
  %2 = call i32 @llvm.hivm.get.TID.X()
  %3 = call i32 @llvm.hivm.get.laneID()
  %4 = call i32 @llvm.hivm.get.BLOCK.DIM.X()
  %5 = call i32 @llvm.hivm.get.BLOCK.DIM.Y()
  %6 = call i32 @llvm.hivm.get.BLOCK.DIM.Z()
  %7 = call i32 @llvm.hivm.get.GRID.DIM.X()
  %8 = call i32 @llvm.hivm.get.GRID.DIM.Y()
  %9 = call i32 @llvm.hivm.get.GRID.DIM.Z()
  %10 = call i32 @llvm.hivm.get.BLOCK.IDX.X()
  %11 = call i32 @llvm.hivm.get.LANEMASK.EQ()
  %12 = call i32 @llvm.hivm.get.LANEMASK.LE()
  %13 = call i32 @llvm.hivm.get.LANEMASK.LT()
  %14 = call i32 @llvm.hivm.get.LANEMASK.GE()
  %15 = call i32 @llvm.hivm.get.LANEMASK.GT()
  %16 = and i32 %3, 1
  %17 = icmp eq i32 %16, 0
  %18 = call i1 @llvm.hivm.vote.all(i1 %17)
  %19 = call i1 @llvm.hivm.vote.any(i1 %17)
  %20 = call i1 @llvm.hivm.vote.uni(i1 %17)
  %21 = zext i1 %18 to i32
  %22 = zext i1 %19 to i32
  %23 = zext i1 %20 to i32
  %24 = call i32 @llvm.hivm.vote.ballot(i1 %17)
  %25 = add i32 %3, 100
  %26 = call i32 @llvm.hivm.shfl.idx.i32(i32 %25, i32 7939)
  %27 = call i32 @llvm.hivm.shfl.up.i32(i32 %25, i32 2)
  %28 = call i32 @llvm.hivm.shfl.down.i32(i32 %25, i32 7938)
  %29 = call i32 @llvm.hivm.shfl.bfly.i32(i32 %25, i32 7937)
  %30 = add i32 %3, 1
  %31 = call i32 @llvm.hivm.redux.add.s32(i32 %30)
  %32 = call i32 @llvm.hivm.redux.max.s32(i32 %30)
  %33 = call i32 @llvm.hivm.redux.min.s32(i32 %30)
  %34 = call float @llvm.hivm.redux.add.f32(float 1.000000e+00)
  %35 = call i32 @llvm.hivm.fp32.to.s32(float %34, i32 0, i32 1)
  %36 = mul i32 %3, 32
  %37 = zext i32 %36 to i64
  %38 = getelementptr i32, ptr addrspace(6) %0, i64 %37
  store i32 %2, ptr addrspace(6) %38, align 4
  %39 = add i32 %36, 1
  %40 = zext i32 %39 to i64
  %41 = getelementptr i32, ptr addrspace(6) %0, i64 %40
  store i32 %3, ptr addrspace(6) %41, align 4
  %42 = add i32 %36, 2
  %43 = zext i32 %42 to i64
  %44 = getelementptr i32, ptr addrspace(6) %0, i64 %43
  store i32 %4, ptr addrspace(6) %44, align 4
  %45 = add i32 %36, 3
  %46 = zext i32 %45 to i64
  %47 = getelementptr i32, ptr addrspace(6) %0, i64 %46
  store i32 %5, ptr addrspace(6) %47, align 4
  %48 = add i32 %36, 4
  %49 = zext i32 %48 to i64
  %50 = getelementptr i32, ptr addrspace(6) %0, i64 %49
  store i32 %6, ptr addrspace(6) %50, align 4
  %51 = add i32 %36, 5
  %52 = zext i32 %51 to i64
  %53 = getelementptr i32, ptr addrspace(6) %0, i64 %52
  store i32 %7, ptr addrspace(6) %53, align 4
  %54 = add i32 %36, 6
  %55 = zext i32 %54 to i64
  %56 = getelementptr i32, ptr addrspace(6) %0, i64 %55
  store i32 %8, ptr addrspace(6) %56, align 4
  %57 = add i32 %36, 7
  %58 = zext i32 %57 to i64
  %59 = getelementptr i32, ptr addrspace(6) %0, i64 %58
  store i32 %9, ptr addrspace(6) %59, align 4
  %60 = add i32 %36, 8
  %61 = zext i32 %60 to i64
  %62 = getelementptr i32, ptr addrspace(6) %0, i64 %61
  store i32 %10, ptr addrspace(6) %62, align 4
  %63 = add i32 %36, 9
  %64 = zext i32 %63 to i64
  %65 = getelementptr i32, ptr addrspace(6) %0, i64 %64
  store i32 %11, ptr addrspace(6) %65, align 4
  %66 = add i32 %36, 10
  %67 = zext i32 %66 to i64
  %68 = getelementptr i32, ptr addrspace(6) %0, i64 %67
  store i32 %12, ptr addrspace(6) %68, align 4
  %69 = add i32 %36, 11
  %70 = zext i32 %69 to i64
  %71 = getelementptr i32, ptr addrspace(6) %0, i64 %70
  store i32 %13, ptr addrspace(6) %71, align 4
  %72 = add i32 %36, 12
  %73 = zext i32 %72 to i64
  %74 = getelementptr i32, ptr addrspace(6) %0, i64 %73
  store i32 %14, ptr addrspace(6) %74, align 4
  %75 = add i32 %36, 13
  %76 = zext i32 %75 to i64
  %77 = getelementptr i32, ptr addrspace(6) %0, i64 %76
  store i32 %15, ptr addrspace(6) %77, align 4
  %78 = add i32 %36, 14
  %79 = zext i32 %78 to i64
  %80 = getelementptr i32, ptr addrspace(6) %0, i64 %79
  store i32 %21, ptr addrspace(6) %80, align 4
  %81 = add i32 %36, 15
  %82 = zext i32 %81 to i64
  %83 = getelementptr i32, ptr addrspace(6) %0, i64 %82
  store i32 %22, ptr addrspace(6) %83, align 4
  %84 = add i32 %36, 16
  %85 = zext i32 %84 to i64
  %86 = getelementptr i32, ptr addrspace(6) %0, i64 %85
  store i32 %23, ptr addrspace(6) %86, align 4
  %87 = add i32 %36, 17
  %88 = zext i32 %87 to i64
  %89 = getelementptr i32, ptr addrspace(6) %0, i64 %88
  store i32 %24, ptr addrspace(6) %89, align 4
  %90 = add i32 %36, 18
  %91 = zext i32 %90 to i64
  %92 = getelementptr i32, ptr addrspace(6) %0, i64 %91
  store i32 %26, ptr addrspace(6) %92, align 4
  %93 = add i32 %36, 19
  %94 = zext i32 %93 to i64
  %95 = getelementptr i32, ptr addrspace(6) %0, i64 %94
  store i32 %27, ptr addrspace(6) %95, align 4
  %96 = add i32 %36, 20
  %97 = zext i32 %96 to i64
  %98 = getelementptr i32, ptr addrspace(6) %0, i64 %97
  store i32 %28, ptr addrspace(6) %98, align 4
  %99 = add i32 %36, 21
  %100 = zext i32 %99 to i64
  %101 = getelementptr i32, ptr addrspace(6) %0, i64 %100
  store i32 %29, ptr addrspace(6) %101, align 4
  %102 = add i32 %36, 22
  %103 = zext i32 %102 to i64
  %104 = getelementptr i32, ptr addrspace(6) %0, i64 %103
  store i32 %31, ptr addrspace(6) %104, align 4
  %105 = add i32 %36, 23
  %106 = zext i32 %105 to i64
  %107 = getelementptr i32, ptr addrspace(6) %0, i64 %106
  store i32 %32, ptr addrspace(6) %107, align 4
  %108 = add i32 %36, 24
  %109 = zext i32 %108 to i64
  %110 = getelementptr i32, ptr addrspace(6) %0, i64 %109
  store i32 %33, ptr addrspace(6) %110, align 4
  %111 = add i32 %36, 25
  %112 = zext i32 %111 to i64
  %113 = getelementptr i32, ptr addrspace(6) %0, i64 %112
  store i32 %35, ptr addrspace(6) %113, align 4
  ret void
}

attributes #0 = { "target-cpu"="dav-c310-vec" "target-features"="+ATOMIC,+ArchV130,+AregRedefinable,+ArithmeticBf16,+AtomicForB8 ,+F8e4m3,+F8e5m2,+F8e8m0,+FFTSBlk,+Fp4e1m2x2,+Fp4e2m1x2,+LDExtRefine,+MOVX8,+MSTX,+SPR7bits,+SyncV,+dav-c310-vec" }
attributes #1 = { noinline "target-cpu"="dav-c310-vec" "target-features"="+ATOMIC,+ArchV130,+AregRedefinable,+ArithmeticBf16,+AtomicForB8 ,+F8e4m3,+F8e5m2,+F8e8m0,+FFTSBlk,+Fp4e1m2x2,+Fp4e2m1x2,+LDExtRefine,+MOVX8,+MSTX,+SPR7bits,+SyncV,+dav-c310-vec" }

!llvm.module.flags = !{!0}
!hivm.annotations = !{!1, !2, !3, !4, !5}

!0 = !{i32 2, !"Debug Info Version", i32 3}
!1 = !{ptr @simt_scalar_core_kernel_mix_aiv, !"kernel", i32 1}
!2 = !{ptr @simt_scalar_core_kernel_mix_aiv, !"kernel_with_simd", i32 1}
!3 = !{ptr @simt_scalar_core_kernel_mix_aiv, !"kernel_with_simt", i32 1}
!4 = distinct !{null, !"simt-max-threads", i32 1024}
!5 = distinct !{null, !"simt-max-registers", i32 32}
!6 = !{!"simt-max-threads", i32 1024}
!7 = !{!"simt-max-registers", i32 32}
