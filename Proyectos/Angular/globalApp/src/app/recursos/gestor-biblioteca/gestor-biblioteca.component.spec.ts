import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { GestorBibliotecaComponent } from './gestor-biblioteca.component';

describe('GestorBibliotecaComponent', () => {
  let component: GestorBibliotecaComponent;
  let fixture: ComponentFixture<GestorBibliotecaComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ GestorBibliotecaComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(GestorBibliotecaComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
